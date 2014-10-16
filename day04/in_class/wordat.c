/*
 * usage: wordat dictionaryname offset
 *
 * Displays the word at offset in the file dictionarynamem
 * or nothing if the character at offset is '\n'.
 */

#include <stdio.h>

int main(int argc, char* argv[])
{
    FILE* fp;
    long theiroffset = 0;
    long myoffset = 0;
    char buffer[1024];

    if (argc < 3) {
        fputs("Not enough args.\n", stderr);
        return 1;
    }

    if ((fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Cannot open '%s'\n", argv[1]);
        return 1;
    }

    if ((sscanf(argv[2], "%li", &theiroffset)) != 1) {
        fprintf(stderr, "'%s' is an invalid offset.\n", argv[2]);
        return 1;
    }

    while(myoffset < theiroffset) {
        fgets(buffer, 1024, fp);
        if (myoffset + strlen(buffer) - 1 == theiroffset) {
            break;
        }
        myoffset += strlen(buffer);
    }
    printf("%s", buffer);

    fclose(fp);
    return 0;
}
