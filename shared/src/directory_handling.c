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
        *errorNumber = errno;
    } else {
        errorNumber = 0;
    }
    return buffer;
}

int ChangeCurrentWorkingDirectory(const char *newDirectory) {
    int errorNumber = 0;
    if (chdir(newDirectory) != 0) {
        errorNumber = errno;
    }
    return errorNumber;
}

int GetListOfFiles(char *directoryPath, char **filenames, int *errorNumber) {
    struct dirent **nameList;
    int n = scandir(directoryPath, &nameList, NULL, alphasort);
    if (n == -1) {
        *errorNumber = errno;
    } else if (n == 0) {
        free(filenames);
    } else if (n > 0) {
        filenames = calloc(n, sizeof(char*));
        for (int i = 0; i < n; ++i) {
            filenames[i] = calloc(256, sizeof(char));
            *filenames[i] = *nameList[i]->d_name;
            free(nameList[i]);
        }
        free(nameList);
    }
    return n;
}

