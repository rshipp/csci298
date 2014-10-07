#include <stdio.h>
#include <string.h>

int main( int argc, char* argv[] )
{
    int i;

    char* breadcrumb;
    for( i=0; i<argc; i++ ) {
        char* theArg = argv[i];
        char* token;

        while( (token = strtok_r( theArg, " \t", &breadcrumb )) ) {
            printf( "The next token is '%s'\n", token );
            theArg = NULL;
        }
    }

    return 0;
}
