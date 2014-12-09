/***
 * Example code for setting flags indicating signal arrival within the context
 * of a curses application.
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "signals.h"

/* REQ9: Stolen from charcell-signal-kip.c */
int sigusr1_received = 0;
int sighup_received = 0;
void signal_catcher( int signum )
{
	switch( signum ) {
		case SIGHUP : sighup_received = 1; break;
		case SIGUSR1 : sigusr1_received = 1; break;
	}
	/***
	 * The "kick in the pants" trick.  We send ourselves a SIGWINCH 
	 * to make sure that getch() in our primary process thread has a "key"
	 * to return.
	 *
	 * Otherwise we would have to wait around until the window *really* changed
	 * or a key press was sent to the terminal.  Either way would result in a clunky user
	 * interface experience.
	 *
	 * Note that raise(2) IS async signal safe (see signal(7)).
	 */
	raise( SIGWINCH );
}


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

	act.sa_handler = signal_catcher;   // set the function to call on signal

	/* prevent reentrant signals --- wise to do whether you are using one or 
	 * multiple signal handler functions
     */
	sigfillset( &act.sa_mask );

	/***
	 * NOTE:  in order for the SIGWINCH "kick in the pants" trick to work, we 
	 * don't want the ncurses read(2) command to restart.  So DON'T set SA_RESTART
	 * for these signal handlers!
	 */

	if( sigaction( signum, &act, NULL )) {
		perror("sigaction" );
		exit(1);
	}
}
