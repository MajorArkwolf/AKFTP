#include "../include/directory_handling.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

char *GetCurrentWorkingDirectory(int *errorNumber) {
    char *buffer = (char *) calloc(FILENAME_MAX, sizeof(char));
    buffer = getcwd(buffer, FILENAME_MAX);
    if (buffer == NULL) {
        errorNumber = errno;
    } else {
        errorNumber = 0;
    }

    return buffer;
}

int ChangeCurrentWorkingDirectory(char *newDirectory) {
    int errorNumber = 0;
    if(chdir(newDirectory) != 0)
    {
        errorNumber = errno;
    }
}

