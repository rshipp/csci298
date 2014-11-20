#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
// don't need the header file

int main()
{
	// don't need to call the setup function

	printf( "pid: %d\n", getpid() );
	char c[3] = { 0, '\n', '\0' };
	c[0] = getc(stdin);
	fputs( c, stdout );
	return 0;
}
