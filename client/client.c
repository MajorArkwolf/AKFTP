
#include "client.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <directory_handling.h>
#include <token.h>
#include <errno.h>

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
        if (numTokens > 0) {
            commandResult = HandleCommand(tokens, numTokens);
            if (commandResult == -1) {
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

int HandleCommand(char **tokens, int numTokens) {

    if (strcmp(tokens[0], "quit") == 0 || strcmp(tokens[0], "exit") == 0) {
        return -1;
    } else if (strcmp(tokens[0], "help") == 0) {
        printf("Not implemented yet, good luck. Type quit or exit to exit program.\n");
        return 0;
    } else if (strcmp(tokens[0], "pwd") == 0) {
        //Request string from server about current working directory
        //max size of request is (FILENAME_MAX * sizeof(char)) if successful
        //return errno otherwise

        int errorNumber = 0;
        //char* serverWorkingDirectory = GetServerWorkingDirectory(&errorNumber);
        if (1 /*serverWorkingDirectory != NULL */) {
            //printf("%s\n", serverWorkingDirectory);
            //free(serverWorkingDirectory);
            return 0;
        } else {
            PrintCWDError(false, errorNumber);
        }

    } else if (strcmp(tokens[0], "lpwd") == 0) {
        int errorNumber = 0;
        char *clientWorkingDirectory = GetCurrentWorkingDirectory(&errorNumber);
        if (clientWorkingDirectory != NULL) {
            printf("%s\n", clientWorkingDirectory);
            free(clientWorkingDirectory);
            return EXIT_SUCCESS;
        } else {
            PrintCWDError(true, errorNumber);
            return EXIT_FAILURE;
        }

    } else if (strcmp(tokens[0], "dir") == 0) {

    } else if (strcmp(tokens[0], "ldir") == 0) {


    } else if (strcmp(tokens[0], "cd") == 0) {

    } else if (strcmp(tokens[0], "lcd") == 0) {
        //TODO Support file paths with spaces
        if (tokens[1] != NULL) {
            int errorNumber = ChangeCurrentWorkingDirectory(tokens[1]);
            if (errorNumber != 0) {
                PrintCHDIRError(true, errorNumber);
            } else {
                return EXIT_SUCCESS;
            }
        }
        return EXIT_FAILURE;
    } else if (strcmp(tokens[0], "get") == 0) {

    } else if (strcmp(tokens[0], "put") == 0) {

    } else {
        printf("Unknown command. Type 'help' to get the list of commands.\n");
    }
    return 0;
}

void PrintCWDError(bool client, int errorNumber) {
    printf("Error: ");
    switch (errorNumber) {
        case EACCES: {
            printf("Client does not have permission to read filename.\n");
        }
            break;
        case EFAULT: {
            printf("Bad pointer address in buffer.\n");
        }
            break;
        case EINVAL: {
            printf("Size argument of getcwd() is zero and buffer is not a null pointer.\n");
        }
            break;
        case ENAMETOOLONG: {
            printf("The size of the null terminated absolute pathname string exceeds the maximum PATH_MAX bytes.\n");
        }
            break;
        case ENOENT: {
            printf("The current working directory has been unlinked.\n");
        }
            break;
        case ENOMEM: {
            printf("Out of memory.\n");
        }
            break;
        case ERANGE: {
            printf("The size argument (MAX_PATH_LENGTH) is less than the absolute pathname of the working directory.\n");
        }
            break;
        default: {
            printf("%d Unknown error\n", errorNumber);
        }
            break;
    }
}

void PrintCHDIRError(bool client, int errorNumber) {
    printf("Error: ");
    switch (errorNumber) {
        case EACCES: {
            printf("Search permission denied for one of the components of the path.\n");
        }
            break;
        case EFAULT: {
            printf("Attempted to set new working directory to path outside of your accessible address space.\n");
        }
            break;
        case EIO: {
            printf("An I/O error has occurred.\n");
        }
            break;
        case ENAMETOOLONG: {
            printf("The length of the pathname give exceeds the maximum length of PATH_MAX.\n");
        }
            break;
        case ENOENT: {
            printf("Attempted to set the current working directory to a nonexistent path.\n");
        }
            break;
        case ENOMEM: {
            printf("Insufficient kernel memory.\n");
        }
            break;
        case ENOTDIR: {
            printf("Path given was not a directory.\n");
        }
            break;
        default: {
            printf("%d Unknown error\n", errorNumber);
        }
            break;
    }
}

