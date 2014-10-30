#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFLEN 1024
void myerror(FILE* fp, const char* functionname, const char* op)
{
    char errbuf[BUFLEN];
    char* es = strerror_r(errno, errbuf, BUFLEN);
    fprintf(fp, "%s (%s): %s\n", functionname, op, es);
}

void show_error(void)
{
    printf("The errno is currently %d\n", errno);
}

int main(int argc, char* argv[])
{
    FILE* fp = fopen("/etc/passwd", "w");
    if (!fp) {
        puts("Shucks, couldn't write /etc/passwd");
        show_error();
        perror("main (write /etc/passwd)");
    }

    // sets errno
    show_error();
    if(chdir("/does/not/exist")) {
        myerror(stderr, __FUNCTION__, "chdir dne");
    }
    show_error();

    // doesn't change errno
    if(chdir("..")) {
        perror("main (chdir dne)");
    }
    show_error();

    return 0;
}
