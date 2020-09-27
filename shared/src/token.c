#include "token.h"
#include <string.h>

int tokenise(char *inputLine, char *token[])
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