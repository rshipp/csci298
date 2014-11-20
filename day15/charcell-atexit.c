/***
 * Example of using atexit() for cleanup
 */

#include <string.h>
#include <ncurses.h>
#include <ctype.h>
#include <unistd.h>

#include "charcell-utils.h"
#include "charcell-atexit-keyhistory.h"

#define BUFLEN 1024
int main(int argc, char *argv[])
{
	initscr();
	noecho();
	cbreak();
	keypad(stdscr,TRUE);
	curs_set(FALSE);

	// set up initial windows
	WINDOW* display = newwin(1, 1, 0, 0 );
	WINDOW* edit = newwin(1,1, 0, 0 );
	int dispheight = size_display( display, edit );

	// setup atexit in key history module
	setup_key_history();

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
		add_key_history( buf );
	}

	// close curses lib, reset terminal
	endwin();

	return 0;
}

