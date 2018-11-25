/*
// Based of:
// Command line reader (header file), version 2
// Sistemas Operativos, DEI/IST/ULisboa 2018-19
*/

#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

/**
Reads up to 'vectorSize' space-separated arguments from the standard input
and saves them in the entries of the 'argVector' argument.
This function returns once enough arguments are read or the end of the line
is reached

Arguments:
 'argVector' should be a vector of char* previously allocated with
 as many entries as 'vectorSize'
 'vectorSize' is the size of the above vector. A vector of size N allows up to
 N-1 arguments to be read; the entry after the last argument is set to NULL.
 'buffer' is a buffer with 'buffersize' bytes, which will be
 used to hold the strings of each argument.

Return value:
 The number of arguments that were read, or -1 if some error occurred.
*/

int parseCommand(char **argVector, int vectorSize, char *buffer, int bufferSize);


/**
 Copies input to buffer until limit is found. Returns the pointer to the character
 that comes after the limit character.
 When using this function, the programmer has to make sure that there is at least
 one character after the limit char.
*/
char* readTillChar(char* buffer, char limit, char* input, int bufferSize);
#endif
