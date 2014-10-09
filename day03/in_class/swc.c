#include <stdio.h>
#include <string.h>

int main( int argc, char* argv[] )
{
    char* breadcrumb;
    const int BUFLEN = 1024;
    char buffer[BUFLEN];

    size_t location = 0;
    while( fgets( buffer, BUFLEN, stdin ) && !feof(stdin) ) {
        size_t thisline_was = strlen(buffer);
        size_t thistoken = location;
        char* theArg = buffer;
        char* token;
        while( (token = strtok_r( theArg, " \n\t", &breadcrumb )) ) {
            printf( "The next token is '%s' at %lu\n", token, token-buffer+location );
            theArg = NULL;
        }
        location += thisline_was;
    }

    return 0;
}
