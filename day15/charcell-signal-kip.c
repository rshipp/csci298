/***
 * Example code for setting flags indicating signal arrival within the context
 * of a curses application.
 */
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#include <ncurses.h>

#include "charcell-utils.h"

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

	// prevent reentrant signals --- wise to do whether you are using one or 
	// multiple signal handler functions
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



#define BUFLEN 1024
int main(int argc, char *argv[])
{
	initscr();
	noecho();
	cbreak();
	keypad(stdscr,TRUE);
	curs_set(FALSE);

	install_handler( SIGHUP );
	install_handler( SIGUSR1 );

	// set up initial windows
	WINDOW* display = newwin(1, 1, 0, 0 );
	WINDOW* edit = newwin(1,1, 0, 0 );
	int dispheight = size_display( display, edit );

	int d = 0;
	char buf[BUFLEN];
	int ch;
	while((ch = getch()) != KEY_F(1)) {
		switch (ch) {
			case KEY_RESIZE:
				dispheight = size_display( display, edit );
				d = 0;
				strncpy( buf, "KEY_RESIZE", BUFLEN );
				mvwprintw( display, d++ + 2, 2, buf );
				d = d % dispheight;
				if( sighup_received ) {
					snprintf( buf, BUFLEN, "Received SIGHUP %lu", (unsigned long)time(NULL) );
					mvwprintw( display, d++ + 2, 2, buf );
					d = d % dispheight;
					sighup_received = 0;
				}
				if( sigusr1_received ) {
					snprintf( buf, BUFLEN, "Received SIGUSR1 %lu", (unsigned long)time(NULL) );
					mvwprintw( display, d++ + 2, 2, buf );
					d = d % dispheight;
					sigusr1_received = 0;
				}
				wrefresh(display);
				break;
			case KEY_LEFT:
				strncpy( buf, "KEY_LEFT", BUFLEN );
				mvwprintw( display, d++ + 2, 2, buf );
				d = d % dispheight;
				wrefresh(display);
				break;
			case KEY_F(2):
				strncpy( buf, "KEY_F(2)", BUFLEN );
				mvwprintw( display, d++ + 2, 2, buf );
				d = d % dispheight;
				wrefresh(display);
				break;
			default :
				if ( isprint(ch) ) {
					snprintf( buf, BUFLEN, "%c", ch );
					mvwprintw( edit, 1, 2, buf );
					wrefresh(edit);
				} else {
					snprintf( buf, BUFLEN, "Unprintable %04x", ch );
					mvwprintw( display, d++ + 2, 2, buf );
					d = d % dispheight;
					wrefresh(display);
				}
				break;
		}
	}

	// close curses lib, reset terminal
	endwin();

	return 0;
}

