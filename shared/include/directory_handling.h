#pragma once

/**
 * Returns the current working directory of the program calling this or prints any errors that occur
 * @return A char* containing the current path to the working directory of the program, or NULL if there was an issue, must be freed manually
 */
char* GetCurrentWorkingDirectory(int* errorNumber);

int ChangeCurrentWorkingDirectory(char* newDirectory);
