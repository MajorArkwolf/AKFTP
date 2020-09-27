#define MAX_NUM_TOKENS 1000
#define tokenSeparators " \t\n" // characters that separate tokens

/**
 * Turns a list of characters into seperated tokens based on tokenSeperators
 * @param inputLine The line of characters to read from and to turn into tokens
 * @param token The already allocated array to place tokenised words into, should have MAX_NUM_TOKENS space allocated
 */
int tokenise(char *inputLine, char *token[]);
