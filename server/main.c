#include "shared.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include <json_object.h>
#include <json_tokener.h>
#include <string.h>
#include "directory_handling.h"
#include <dirent.h>

#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold

static bool keepRunning = true;

void sigchld_handler(int s) {
    (void) s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

void sigchld_handler_term(int s) {
    if (s > 0) {
        keepRunning = false;
    }
}

json_object *HandleRequest(json_object *json) {
    json_object *response = json_object_new_object();
    const char *command = unpack_command_from_json(json);
    int32_t errorNumber = 0;
    if (strcmp(command, "pwd") == 0) {
        json_object *cwd = json_object_new_string(GetCurrentWorkingDirectory(&errorNumber));
        json_object *error = json_object_new_int(errorNumber);
        json_object_object_add(response, "error", error);
        json_object_object_add(response, "cwd", cwd);
    } else if (strcmp(command, "cd") == 0) {
        const char *dir_change = json_object_get_string(json_object_object_get(json, "dir"));
        errorNumber = ChangeCurrentWorkingDirectory(dir_change);
        json_object *error = json_object_new_int(errorNumber);
        json_object_object_add(response, "error", error);
    } else if (strcmp(command, "dir") == 0) {
        int directoryErrorNum = 0;
        char *currentDirectory = GetCurrentWorkingDirectory(&errorNumber);
        if (currentDirectory == NULL) {
            //-1 sucess status means getting current working directory failed
            json_object *successStatus = json_object_new_int(-1);
            json_object *directoryError = json_object_new_int(errorNumber);
            json_object_object_add(response, "directoryError", directoryError);
            json_object_object_add(response, "status", successStatus);
        } else {
            int size = 0;
            int scandirError = 0;
            char **fileList = GetListOfFiles(currentDirectory, &size, &scandirError);
            json_object *array = json_object_new_array_ext(size);
            for (int i = 0; i < size; ++i) {
                json_object_array_add(array, json_object_new_string(fileList[i]));
                free(fileList[i]);
            }
            json_object_object_add(response, "currentDirectory", json_object_new_string(currentDirectory));
            json_object_object_add(response, "array", array);
            json_object_object_add(response, "scandirError", json_object_new_int(scandirError));
            json_object_object_add(response, "directoryError", json_object_new_int(directoryErrorNum));
            json_object_object_add(response, "arraySize", json_object_new_int(size));
            free(currentDirectory);
            free(fileList);

        }
    } else if (strcmp(command, "put") == 0) {
        const char *filename = json_object_get_string(json_object_object_get(json, "filename"));
        json_object *json_filename = json_object_new_string(filename);
        json_object_object_add(response, "filename", json_filename);
        errorNumber = deserialize_file(json, response);
        json_object *error_json = json_object_new_int(errorNumber);
        json_object_object_add(response, "error", error_json);
    } else if (strcmp(command, "get") == 0) {
        const char *filename = json_object_get_string(json_object_object_get(json, "filename"));
        json_object *json_filename = json_object_new_string(filename);
        json_object_object_add(response, "filename", json_filename);
        errorNumber = serialize_file(response);
        json_object *error_json = json_object_new_int(errorNumber);
        json_object_object_add(response, "error", error_json);
    } else {
        if (response == NULL) {
            response = json_object_new_object();
        }
        json_object *error = json_object_new_int(errorNumber);
        json_object_object_add(response, "error", error);
    }
    return response;
}

void StartConnection(int socket) {
    char *buf = NULL;
    int numbytes = 0;
    json_object *json = NULL;
    json_object *response = NULL;
    int error = 0;
    const char * cwd = GetCurrentWorkingDirectory(&error);
    while (1) {
        if ((numbytes = receive_large(socket, &buf, 0)) == -1) {
            perror("recv");
            close(socket);
            exit(1);
        }
        json = json_tokener_parse(buf);
        if (json != NULL) {
            response = HandleRequest(json);
            size_t size = 0;
            const char *data = json_object_to_json_string_length(response, 0, &size);
            send_large(socket, data, size, 0);
        }
        log_to_file(cwd, json, response, socket);
        if (json != NULL) {
            //while (json_object_put(response) > 1) {}
        }
        if (response != NULL) {
            //while (json_object_put(response) > 1) {}
        }
    }
}

int main(void) {
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
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
    for (p = servinfo; p != NULL; p = p->ai_next) {
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

    if (p == NULL) {
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
    pid_t child_procceses[1000];
    size_t num_of_child_processes = 0;
    signal(SIGINT, sigchld_handler_term);
    while (keepRunning) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
        if (new_fd == -1) {
            perror("failed to connect");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *) &their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);
        pid_t new_proc = fork();
        if (new_proc == 0) { // this is the child process
            close(sockfd);
            StartConnection(new_fd);
        } else {
            child_procceses[num_of_child_processes] = new_proc;
            ++num_of_child_processes;
        }
        close(new_fd);  // parent doesn't need this
    }
    printf("Killing all Child Processes...\n");
    for (size_t i = 0; i < num_of_child_processes; ++i) {
        kill(child_procceses[i], SIGQUIT);
    }
    printf("Childreen have been reaped. Have a good day.\n");

    return EXIT_SUCCESS;
}
