/***
 * example interfaces to Linux signal handling
 */

#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "signal_example_utils.h"


#define SIGHISTORY_SIZE 32
struct sighistory {
	int signo;
	time_t last_handled;
} sighistory_array[SIGHISTORY_SIZE];

int history = 0; // index into history array

void setup_signal_utils()
{
	memset( sighistory_array, 0, sizeof(sighistory_array) );
}

void signal_handler( int signum )
{
	// time is safe inside a signal handler, see signal(7)
	time( &sighistory_array[history].last_handled );
	sighistory_array[history].signo = signum;

	// go to the next one
	++history;
	history %= SIGHISTORY_SIZE;
}

// utility wrapper
void install_handler( int signum )
{
	/***
	 * setup a signal handler --- always use sigaction, avoid signal(2)
	 *
	 * another equally valid approach is to setup an independent handler for
	 * each signal.  one handler seems easier to explain in lecture.
	 */
	struct sigaction act;
	memset( &act, 0, sizeof(act) );

	act.sa_handler = signal_handler;   // set the function to call on signal

	// prevent reentrant signals --- wise to do whether you are using one or 
	// multiple signal handler functions
	sigfillset( &act.sa_mask );

	// restart certain system calls automatically (we do what we can)
	act.sa_flags = SA_RESTART;

	if( sigaction( signum, &act, NULL )) {
		perror("SIGALRM" );
		exit(1);
	}
}

void print_signal_history(FILE* fp)
{
	/** block signal handling */
	sigset_t Set;
	sigfillset(&Set);   // let the kernel sort them out
	sigprocmask(SIG_BLOCK, &Set, NULL);
	
	/** where to begin? */
	int h;
	if( sighistory_array[history].last_handled == 0 ) {
		h = 0;
	} else {
		h = history;
	}

	/***
	 * print history
	 */
	time_t start_time = sighistory_array[h].last_handled;
	if( start_time ) {
		do {
			fprintf( fp, "signal %s last delivered at %lu\n", strsignal(sighistory_array[h].signo),
					sighistory_array[h].last_handled );
			++h;
			h %= SIGHISTORY_SIZE;
		} while( sighistory_array[h].last_handled && sighistory_array[h].last_handled != start_time );
	} else {
		fputs( "no signals handled thus far.\n", fp );
	}


	/***
	 * return signal handling stat to unblocked
	 */
	sigprocmask(SIG_UNBLOCK, &Set, NULL);
}


