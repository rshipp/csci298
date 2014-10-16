#include <stdio.h>

int main(int argc, char* argv[])
{
    FILE* fp = stdout;
    int i;
    for (i=1; i<argc; ++i)
    {
        fprintf(fp, ">> fd %d << ", fileno(fp));
        fputs(argv[i], fp);
        fputs("\n", fp);

        if (i % 2)
        {
            fp = stderr;
        }
        else
        {
            fp = stdout;
        }
    }
    
    return 0;
}
