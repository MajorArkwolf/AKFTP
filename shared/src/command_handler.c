#include "../include/command_handler.h"
#include <string.h>
#include <stdio.h>

int HandleCommand(char **tokens, int numTokens) {

    if (strcmp(tokens[0], "quit") == 0 || strcmp(tokens[0], "exit") == 0) {
        return -1;
    } else if (strcmp(tokens[0], "help") == 0) {
        printf("Not implemented yet, good luck. Type quit or exit to exit program.\n");
        return 0;
    }
    return 0;
}
