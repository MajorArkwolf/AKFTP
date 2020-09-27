
#include "Client.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define MESSAGE_SIZE 1024

int RunClient() {
    bool shouldContinue = true;
    char Message[MESSAGE_SIZE];
    char *tokens[1024];
    int handleMessageReturn = 0;
    printf("Peter and Kiera FTP Client, type 'help' for instructions\n");

    //Program main loop
    while (shouldContinue == true) {
        handleMessageReturn = 0;
        printf(">");
        fgets(Message, MESSAGE_SIZE, stdin);
        strtok(Message, "\n");
        handleMessageReturn = HandleMessage(Message);
        if (handleMessageReturn != 0) {
            if (handleMessageReturn == -1) {
                shouldContinue = false;
            }
        }
    }
    //Close stuff
    exit(EXIT_SUCCESS);
}

int HandleMessage(char **tokens) {


}


