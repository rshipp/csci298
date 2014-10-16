#include <stdio.h>

int main(int argc, char* argv[])
{
    int i;
    for (i=1; i<argc; ++i)
    {
        if (i % 2)
        {
            printf(">> fd %d << ", fileno(stdout));
            puts(argv[i]);
        }
        else
        {
            fprintf(stderr, ">> fd %d << ", fileno(stderr));
            fputs(argv[i], stderr);
            fputs("\n", stderr);
        }
    }
    
    return 0;
}
