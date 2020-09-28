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

int RunClient() {
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
    RunClient();
}
