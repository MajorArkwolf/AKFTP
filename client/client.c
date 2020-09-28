#include "client.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <command_handler.h>
#include <token.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <json_object.h>
#include <json_tokener.h>
#include "shared.h"

#define MESSAGE_SIZE 1024

#define PORT "3490" // the port client will be connecting to

int RunClient(int socket) {
    bool shouldContinue = true;
    char message[MESSAGE_SIZE];
    char *tokens[MAX_NUM_TOKENS];

    printf("Peter and Kiera FTP Client, type 'help' for instructions\n");

    //Program main loop
    while (shouldContinue == true) {
        int numTokens = 0;
        int commandResult = 0;

        printf(">");
        fgets(message, MESSAGE_SIZE, stdin);
        strtok(message, "\n");
        numTokens = Tokenise(message, tokens, "\n\t ");
        if (numTokens > 0)
        {
            commandResult = HandleCommand(tokens, numTokens) ;
            if(commandResult == -1)
            {
                shouldContinue = false;
            }
        }

    }
    //Close stuff
    exit(EXIT_SUCCESS);
}

int StartClient(int argc, char **argv) {
    int sockfd, numbytes;
    char *buf = NULL;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr, "usage: client hostname\n");
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
    for (p = servinfo; p != NULL; p = p->ai_next) {
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

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
    RunClient(sockfd);
    return 0;
}

