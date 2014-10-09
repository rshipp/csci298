#include <stdio.h>
#include <string.h>

int main( int argc, char* argv[] )
{
    int i;

    for( i=0; i<argc; i++ ) {
        fputs( argv[i], stderr );
        fputs( "\n", stderr );
    }

    return 0;
}
