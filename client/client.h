#pragma once

/**
 * The main loop of the client program, asks for user input and sends it where it needs to go
 * @return EXIT_SUCCESS or EXIT_FAILURE based on what happens during the runtime the program
 */
int RunClient();

/**
 * Starts the client program and sorts the command line arguments into an ip address and port number
 * @param argc Number of command line arguments passed in from main
 * @param argv The command line arguments passed in from main
 * @return EXIT_SUCCESS or EXIT_FAILURE based on what happens during the runtime the program
 */
int StartClient(int argc, char** argv);


