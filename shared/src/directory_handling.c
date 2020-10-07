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

char** GetListOfFiles(char *directoryPath, int *size, int *errorNumber) {
    struct dirent **nameList;
    char **filenames = NULL;
    int n = scandir(directoryPath, &nameList, NULL, alphasort);
    *size = n;
    if (n == -1) {
        *errorNumber = errno;
    } else if (n == 0) {
        return NULL;
    } else if (n > 0) {
        filenames = (char**) calloc(n, sizeof(char*));
        for (int i = 0; i < n; ++i) {
            filenames[i] = (char*)calloc(FILENAME_MAX, sizeof(char));
            strcpy(filenames[i], nameList[i]->d_name);
            free(nameList[i]);
        }
        free(nameList);
    }
    return filenames;
}

