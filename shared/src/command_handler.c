#include "../include/command_handler.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

int HandleCommand(char **tokens, int numTokens) {

    if (strcmp(tokens[0], "quit") == 0 || strcmp(tokens[0], "exit") == 0) {
        return 1;
    } else if (strcmp(tokens[0], "help") == 0) {
        printf("Not implemented yet, good luck. Type quit or exit to exit program.\n");
        return 0;
    } else if (strcmp(tokens[0], "pwd") == 0) {


    } else if (strcmp(tokens[0], "lpwd") == 0) {
        char *clientWorkingDirectory = GetClientWorkingDirectory();
        if (clientWorkingDirectory != NULL) {
            printf("%s\n", clientWorkingDirectory);
            free(clientWorkingDirectory);
            return 0;
        } else {
            return -2;
        }

    } else if (strcmp(tokens[0], "dir") == 0) {

    } else if (strcmp(tokens[0], "ldir") == 0) {

    } else if (strcmp(tokens[0], "cd") == 0) {

    } else if (strcmp(tokens[0], "lcd") == 0) {

    } else if (strcmp(tokens[0], "get") == 0) {

    } else if (strcmp(tokens[0], "put") == 0) {

    }
    return 0;
}

char *GetClientWorkingDirectory() {
    char*buffer = (char*)calloc(FILENAME_MAX, sizeof(char));
    buffer = getcwd(buffer, PATH_MAX);
    if (buffer == NULL) {
        int errorValue = errno;
        printf("Error: ");
        switch (errorValue) {
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
        }

    }
    return buffer;
}
