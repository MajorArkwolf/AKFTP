#include "../include/token.h"
#include <string.h>

int Tokenise(char *inputLine, char *token[],  char* separators)
{
    char *readToken = strtok(inputLine, separators);
    int count = 0;

    token[count] = readToken;
    while (readToken != NULL)
    {
        ++count;
        if (count >= MAX_NUM_TOKENS)
        {
            return -1;
        }
        readToken = strtok(NULL, separators);
        token[count] = readToken;
    }
    return count;
}
