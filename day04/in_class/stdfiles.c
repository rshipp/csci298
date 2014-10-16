#include <stdio.h>

int main(int argc, char* argv[])
{
    int i;
    for (i=1; i<argc; ++i)
    {
        if (i % 2)
        {
            puts(argv[i]);
        }
        else
        {
            fputs(argv[i], stderr);
            fputs("\n", stderr);
        }
    }
    
    return 0;
}
