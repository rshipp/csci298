#include <stdio.h>
#include <string.h>

int main( int argc, char* argv[] )
{
    int i;

#if 0
    char* breadcrumb;
    for( i=0; i<argc; i++ ) {
        char* theArg = argv[i];
        char* token;

        while( (token = strtok_r( theArg, " \t", &breadcrumb )) ) {
            printf( "The next token is '%s'\n", token );
            theArg = NULL;
        }
    }
#endif

    const int BUFLEN = 1024;
    char buffer[BUFLEN];

    while( fgets( buffer, BUFLEN, stdin ) && !feof(stdin) ) {
        char* theArg = buffer;
        char* token;
        while( (token = strtok_r( theArg, " \t", &breadcrumb )) ) {
            printf( "The next token is '%s'\n", token );
            theArg = NULL;
        }
    }

    return 0;
}
