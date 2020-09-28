#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "shared.h"
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include <json_object.h>
#include <json_tokener.h>
#include <string.h>
#include "directory_handling.h"

#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 10	 // how many pending connections queue will hold

void sigchld_handler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

json_object *HandleRequest(json_object *json) {
    json_object *response = json_object_new_object();
    const char *command = unpack_command_from_json(json);
    if(strcmp(command, "pwd") == 0) {
        int errorNumber = 0;
        json_object *cwd = json_object_new_string(GetCurrentWorkingDirectory(&errorNumber));
        json_object *error = json_object_new_int(errorNumber);
        json_object_object_add(response, "error", error);
        json_object_object_add(response, "cwd", cwd);
    }
    return response;
}

int main(void)
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (/**!fork()**/ 1) { // this is the child process
            bool close_program = false;
            char *buf = NULL;
            int numbytes = 0;
            close(sockfd);
            while(!close_program) {
                if ((numbytes = receive_large(new_fd, &buf,0)) == -1) {
                    perror("recv");
                    close(new_fd);
                    exit(1);
                }
                json_object *json = json_tokener_parse(buf);
                json_object *response = HandleRequest(json);
                size_t size;
                const char *data = json_object_to_json_string_length(response, 0, &size);
                send_large (new_fd, data, size, 0);
//                if (numbytes > 1) {
//                    buf[numbytes] = '\0';
//                    printf("Server: %s\n", str);
//                    send_large (new_fd, data, size, 0);
//                }
//                numbytes = 0;
            }
        }
        close(new_fd);  // parent doesn't need this
    }
    return EXIT_SUCCESS;
}
