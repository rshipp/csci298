#include <stdio.h>
#include <string.h>

int main()
{
    char aChar = '=', anotherChar = '\n';

    puts( "My name is mud" );

    // a string in C is simply an array of chars
    char mud[] = "My name is mud";
    puts( mud );

    printf( "The string is %lu bytes long /%s/.\n", strlen(mud), mud );

    mud[11] = '\0';
    printf( "The string is %lu bytes long /%s/.\n", strlen(mud), mud );

    mud[11] = 'M';
    printf( "The string is %lu bytes long /%s/.\n", strlen(mud), mud );

    return 0;
}
