#include "../include/CommandHandler.h"
#include <string.h>
#include <stdio.h>
int HandleCommand(char **tokens) {

    if (strcmp(tokens[0], "quit") == 0 || strcmp(tokens[0], "exit") == 0) {
        return -1;
    } else if (strcmp(tokens[0], "help") == 0) {
        printf("Not implemented yet, good luck. Type quit or exit to destroy program\n");
    }
    return 0;
}
