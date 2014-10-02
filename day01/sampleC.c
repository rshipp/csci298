#include <stdio.h>

int main()
{
    int c, width;

    puts( "Enter a character" );
    if( (c=getc(stdin)) == EOF ) return 1;
    puts( "Enter a width" );
    if( scanf( "%i", &width ) != 1 ) return 1;
    if ( width > 0 && width % 2 == 1) {
        printf( "Read character '%c' and width '%i'\n", c, width );
        puts( "Thanks you for your support." );
    } else {
        return 1;
    }

    return 0;
}
