#pragma once
struct dirent;

/**
 * Returns the current working directory of the program calling this or NULL if an error occurred
 * @param errorNumber A pointer to an integer to set to the value of the error number returned by getcwd
 * @return A char* containing the current path to the working directory of the program, or NULL if there was an issue, must be freed manually
 */
char *GetCurrentWorkingDirectory(int *errorNumber);

/**
 * Changes the current working directory of the program that calls this to the directory specified in the arguments.
 * @param newDirectory The relative or full path of the directory to set the current working directory of the program to
 * @return 0 If success, error number set to errno by chdir otherwise
 */
int ChangeCurrentWorkingDirectory(const char *newDirectory);

/**
 * Takes in a directory path, and a char** array to place file names found into.
 * @param directoryPath The path of the directory to read a list of files from
 * @param filenames A 2-dimensional array containing all
 * @param errorNumber The error number set into errno by
 * @return The number of filenames found and placed into the filenames array, -1 if an error occurred
 */
char** GetListOfFiles(char *directoryPath, int *size, int *errorNumber);