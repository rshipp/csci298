#include <stdio.h>
#include <string.h>

int main( int argc, char* argv[] )
{
    int i;
    if( argc != 2 ) { exit(1); }
    char* query = argv[1];

    char* breadcrumb;
    const int BUFLEN = 1024;
    char buffer[BUFLEN];

    while( fgets( buffer, BUFLEN, stdin ) && !feof(stdin) ) {
        char* theArg = buffer;
        char* token;
        while( (token = strtok_r( theArg, " \t", &breadcrumb )) ) {
            printf( "The next token is '%s'\n", token );
            if( !strcmp(query, token) ) { puts(token); }
            theArg = NULL;
        }
    }

    return 0;
}
