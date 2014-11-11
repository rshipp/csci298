/***
 * signal_example.c demonstrates how to use sigaction(2) to manage xterm
 * resizing in ncurses, as well as other standard and rather simple 
 * signal patterns:
 *
 *   SIGALRM,
 *   SIGUSR1,
 *   SIGHUP,
 *   SIGTERM,
 *   SIGFPE,
 *
 * see signal(7) for many, many, gory details.
 */

#include <signal.h>
#include <unistd.h>  // pause
#include <stdlib.h>  

#include "signal_example_utils.h"

int main()
{
	setup_signal_utils();

	install_handler( SIGUSR1 );
	install_handler( SIGHUP );

	do {
		fputs( "pausing for signal delivery...\n", stdout );
		fflush( stdout );
		printf( "%u seconds remaining in sleep() call\n", sleep(7) );
		print_signal_history( stdout );
		fputs( "\n", stdout );
	} while( 1 );

	teardown_signal_utils();
	return 0;
}

