#include "../include/Token.h"
#include <string.h>

int Tokenise(char *inputLine, char *token[],  char* separators)
{
    char *readToken;
    int count = 0;

    readToken = strtok(inputLine, tokenSeparators);
    token[count] = readToken;
    while (readToken != NULL)
    {
        ++count;
        if (count >= MAX_NUM_TOKENS)
        {
            return -1;
        }
        readToken = strtok(NULL, tokenSeparators);
        token[count] = readToken;
    }
    return count;
}