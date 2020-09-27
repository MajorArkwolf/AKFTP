#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <json_object.h>
#include <json_tokener.h>
#include "shared.h"

#define PORT "3490" // the port client will be connecting to

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return EXIT_FAILURE;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
    while (1) {
        struct json_object *json = json_object_new_object();
        char str[50];
        scanf("%s", str);
        json_object *json_string = json_object_new_string(str);
        json_object_object_add(json, "command", json_string);

        //send(sockfd, str, 50, 0);
        size_t size;
        const char *data = json_object_to_json_string_length(json, 0, &size);
//        json_object *j1 = json_tokener_parse(data);
//        const char *de = json_object_to_json_string(json_object_object_get(j1, "command"));
//        printf("size: %zu, data: %s\n", size, de);
        send_large(sockfd, &data, size, 0);

        if ((numbytes = receive_large(sockfd, buf, 0)) == -1) {
            perror("recv");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        printf("client: received '%s'\n", buf);
    }
    close(sockfd);

    return EXIT_SUCCESS;
}
