#pragma once

/**
 * Takes in a 2d array of characters that have been separated into tokens and performs the actions appropriate to them
 * @param tokens The 2D character array of tokens to perform actions based on
 * @param numTokens The number of tokens in the array
 * @return -1 If exit or quit called, 0 otherwise
 */
int HandleCommand(char **tokens, int numTokens);

/**
 * Returns the current working directory of the client or prints any errors that occur
 * @return A char* containing the current path to the working directory of the client, or NULL if there was an issue
 */
char* GetClientWorkingDirectory();

char* GetServerWorkingDirectory();

