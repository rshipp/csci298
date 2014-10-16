#include <stdio.h>

int main(int argc, char* argv[])
{
    int i;
    for (i=1; i<argc; ++i)
    {
        if (i % 2 == 0)
        {
            fputs(argv[i], stderr);
            fputs("\n", stderr);
        }
        else
        {
            puts(argv[i]);
        }
    }
    
    return 0;
}
