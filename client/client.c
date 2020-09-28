
#include "client.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <command_handler.h>
#include <token.h>

#define MESSAGE_SIZE 1024

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
            if(commandResult == 1)
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

