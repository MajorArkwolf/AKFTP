#pragma once
#include <stdbool.h>
#include <json_object.h>
/**
 * The main loop of the client program, asks for user input and sends it where it needs to go
 * @return EXIT_SUCCESS or EXIT_FAILURE based on what happens during the runtime the program
 */
int RunClient(int socket);

/**
 * Starts the client program and sorts the command line arguments into an ip address and port number
 * @param argc Number of command line arguments passed in from main
 * @param argv The command line arguments passed in from main
 * @return EXIT_SUCCESS or EXIT_FAILURE based on what happens during the runtime the program
 */
int StartClient(int argc, char** argv);

/**
 * Takes in a 2d array of characters that have been separated into tokens and performs the actions appropriate to them
 * @param tokens The 2D character array of tokens to perform actions based on
 * @param numTokens The number of tokens in the array
 * @return -1 If exit or quit called, 0 otherwise
 */
int HandleCommand(json_object *json, int socket, char **tokens, int numTokens);

/**
 * Prints the description related with the error number received by the getcwd function
 * @param client True if the client received the error number when getting the current directory, false if the server received it
 * @param errorNumber The error number set to errno by getcwd
 */
void PrintCWDError(bool client, int errorNumber);

/**
 * Prints the description related with the error number received by the chdir function
 * @param client True if the client received the error number when setting the current directory, false if the server received it
 * @param errorNumber The error number set to errno by chdir
 */
void PrintCHDIRError(bool client, int errorNumber);

void PrintSCANDIRError(bool client, int errorNumber);