#include "client.h"
#include <shared.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <directory_handling.h>
#include <token.h>
#include <errno.h>
#include <dirent.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <json_tokener.h>
#include <json.h>

#define MESSAGE_SIZE 1024

#define PORT "3490" // the port client will be connecting to

int RunClient(int socket) {
    bool shouldContinue = true;
    char message[MESSAGE_SIZE];
    char *tokens[MAX_NUM_TOKENS];

    printf("Peter and Kiera FTP Client, type 'help' for instructions\n");
    json_object *json = NULL;
    //Program main loop
    while (shouldContinue == true) {
        int numTokens = 0;
        int commandResult = 0;

        size_t size;
        printf(">");
        fgets(message, MESSAGE_SIZE, stdin);
        strtok(message, "\n");
        numTokens = Tokenise(message, tokens, "\n\t ");
        json = json_object_new_object();
        if (numTokens > 0) {
            commandResult = HandleCommand(json, socket, tokens, numTokens);
            if (commandResult == -1) {
                shouldContinue = false;
            }
        }
        free(json);
    }
    //Close stuff
    exit(EXIT_SUCCESS);
}

int StartClient(int argc, char **argv) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    /*if (argc != 2) {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }*/

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return EXIT_FAILURE;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
    RunClient(sockfd);
    return 0;
}

int HandleCommand(json_object *json, int socket, char **tokens, int numTokens) {
    size_t size = 0;
    char *response_data = NULL;
    if (strcmp(tokens[0], "quit") == 0 || strcmp(tokens[0], "exit") == 0) {
        return -1;
    } else if (strcmp(tokens[0], "help") == 0) {
        printf("Not implemented yet, good luck. Type quit or exit to exit program.\n");
        return 0;
    } else if (strcmp(tokens[0], "pwd") == 0) {
        pack_command_to_json(json, "pwd");
        const char *data = json_object_to_json_string_length(json, 0, &size);
        send_large(socket, data, size, 0);
        receive_large(socket, &response_data, 0);
        json_object *response = json_tokener_parse(response_data);
        int errorNumber = json_object_get_int((json_object_object_get(response, "error")));
        //char* serverWorkingDirectory = GetServerWorkingDirectory(&errorNumber);
        if (errorNumber == 0) {
            const char *cwd = json_object_get_string(json_object_object_get(response, "cwd"));
            printf("%s\n", cwd);
            free(response);
            return 0;
        } else {
            PrintCWDError(false, errorNumber);
        }

    } else if (strcmp(tokens[0], "lpwd") == 0) {
        int errorNumber = 0;
        char *clientWorkingDirectory = GetCurrentWorkingDirectory(&errorNumber);
        if (clientWorkingDirectory != NULL) {
            printf("%s\n", clientWorkingDirectory);
            free(clientWorkingDirectory);
            return EXIT_SUCCESS;
        } else {
            PrintCWDError(true, errorNumber);
            return EXIT_FAILURE;
        }

    } else if (strcmp(tokens[0], "dir") == 0) {
        pack_command_to_json(json, "dir");
        const char *data = json_object_to_json_string_length(json, 0, &size);
        send_large(socket, data, size, 0);
        receive_large(socket, &response_data, 0);
        json_object *response = json_tokener_parse(response_data);
        int errorNumber = json_object_get_int((json_object_object_get(response, "directoryError")));
        if (errorNumber == 0) {
            int scandirError = 0;
            scandirError = json_object_get_int((json_object_object_get(response, "scandirError")));
            if (scandirError == 0) {
                const int arraySize = json_object_get_int(json_object_object_get(response, "arraySize"));
                const struct array_list *array = json_object_get_array(json_object_object_get(response, "array"));
                const char* serverDirectory = json_object_get_string(json_object_object_get(response, "currentDirectory"));
                printf("(%s)\n", serverDirectory);
                for (int i = 0; i < arraySize; ++i) {
                    printf("%s\n", json_object_get_string((array->array)[i]));
                }
                array->free_fn((void *) array);
            } else {
                PrintSCANDIRError(false, scandirError);
            }

            free(response);

            return 0;
        } else {
            PrintCWDError(false, errorNumber);
        }


    } else if (strcmp(tokens[0], "ldir") == 0) {
        int errorNumber = 0;
        int count = 0;
        char *clientWorkingDirectory = GetCurrentWorkingDirectory(&errorNumber);
        if (clientWorkingDirectory != NULL) {
            char **filenames = NULL;
            filenames = GetListOfFiles(clientWorkingDirectory, &count, &errorNumber);
            if (errorNumber == 0) {
                if (count > 0) {
                    if (filenames != NULL) {
                        for (int i = 0; i < count; ++i) {
                            if (filenames[i] != NULL) {
                                printf("%s\n", filenames[i]);
                                free(filenames[i]);
                            }
                        }
                        free(filenames);
                    }
                }
            } else
            {
                PrintSCANDIRError(true, errorNumber);
                return EXIT_FAILURE;
            }
            free(clientWorkingDirectory);
            return EXIT_SUCCESS;
        } else {
            PrintCWDError(true, errorNumber);
            return EXIT_FAILURE;
        }

    } else if (strcmp(tokens[0], "cd") == 0) {
        //TODO Support file paths with spaces
        if (tokens[1] != NULL) {
            pack_command_to_json(json, "cd");
            json_object *dir = json_object_new_string(tokens[1]);
            json_object_object_add(json, "dir", dir);
            const char *data = json_object_to_json_string_length(json, 0, &size);
            send_large(socket, data, size, 0);
            receive_large(socket, &response_data, 0);
            //send tokens[1] to server with command to change directory
            //server will return error number, 0 if no error
            json_object *response = json_tokener_parse(response_data);
            int errorNumber = json_object_get_int((json_object_object_get(response, "error")));
            if (errorNumber != 0) {
                PrintCHDIRError(false, errorNumber);
            } else {
                return EXIT_SUCCESS;
            }
        }
        return EXIT_FAILURE;

    } else if (strcmp(tokens[0], "lcd") == 0) {
        //TODO Support file paths with spaces
        if (tokens[1] != NULL) {
            int errorNumber = ChangeCurrentWorkingDirectory(tokens[1]);
            if (errorNumber != 0) {
                PrintCHDIRError(true, errorNumber);
            } else {
                return EXIT_SUCCESS;
            }
        }
        return EXIT_FAILURE;
    } else if (strcmp(tokens[0], "get") == 0) {

    } else if (strcmp(tokens[0], "put") == 0) {

    } else {
        printf("Unknown command. Type 'help' to get the list of commands.\n");
    }
    return 0;
}

void PrintCWDError(bool client, int errorNumber) {
   if(client == true)
   {
       printf("(Client)");
   } else
   {
       printf("(Server)");
   }
    printf("Error: ");
    switch (errorNumber) {
        case EACCES: {
            printf("Client does not have permission to read filename.\n");
        }
            break;
        case EFAULT: {
            printf("Bad pointer address in buffer.\n");
        }
            break;
        case EINVAL: {
            printf("Size argument of getcwd() is zero and buffer is not a null pointer.\n");
        }
            break;
        case ENAMETOOLONG: {
            printf("The size of the null terminated absolute pathname string exceeds the maximum PATH_MAX bytes.\n");
        }
            break;
        case ENOENT: {
            printf("The current working directory has been unlinked.\n");
        }
            break;
        case ENOMEM: {
            printf("Out of memory.\n");
        }
            break;
        case ERANGE: {
            printf("The size argument (MAX_PATH_LENGTH) is less than the absolute pathname of the working directory.\n");
        }
            break;
        default: {
            printf("%d Unknown error\n", errorNumber);
        }
            break;
    }
}

void PrintCHDIRError(bool client, int errorNumber) {
    if(client == true)
    {
        printf("(Client)");
    } else
    {
        printf("(Server)");
    }
    printf("Error: ");
    switch (errorNumber) {
        case EACCES: {
            printf("Search permission denied for one of the components of the path.\n");
        }
            break;
        case EFAULT: {
            printf("Attempted to set new working directory to path outside of your accessible address space.\n");
        }
            break;
        case EIO: {
            printf("An I/O error has occurred.\n");
        }
            break;
        case ENAMETOOLONG: {
            printf("The length of the pathname give exceeds the maximum length of PATH_MAX.\n");
        }
            break;
        case ENOENT: {
            printf("Attempted to set the current working directory to a nonexistent path.\n");
        }
            break;
        case ENOMEM: {
            printf("Insufficient kernel memory.\n");
        }
            break;
        case ENOTDIR: {
            printf("Path given was not a directory.\n");
        }
            break;
        default: {
            printf("%d Unknown error\n", errorNumber);
        }
            break;
    }
}

void PrintSCANDIRError(bool client, int errorNumber) {
    if(client == true)
    {
        printf("(Client)");
    } else
    {
        printf("(Server)");
    }
    printf("Error: ");
    switch (errorNumber) {
        case ENOENT: {
            printf("Unknown issue with current working directory.\n");
        }
            break;
        case ENOMEM: {
            printf("Insufficient memory to complete the operation.\n");
        }
            break;
        case ENOTDIR: {
            printf("Path given was not a directory.\n");
        }
            break;
        default: {
            printf("%d Unknown error\n", errorNumber);
        }
            break;
    }
}
