/***
 * This example code started as an ungracious rip from
 *    http://viget.com/extend/c-games-in-ncurses-using-multiple-windows
 * khellman@mines.edu
 *
 * Handy references online:
 *  a. http://pubs.opengroup.org/onlinepubs/7908799/xcurses/curses.h.html
 *  b. http://www.tldp.org/HOWTO/html_single/NCURSES-Programming-HOWTO/
 *  c. http://www.linuxfocus.org/English/March2002/article233.shtml#233lfindex2
 *  d. http://invisible-island.net/ncurses/ncurses-intro.html
 *
 */
#include <string.h>
#include <ncurses.h>
#include <ctype.h>
#include <unistd.h>

#define CORNER '+'
#define VERT1  '|'
#define VERT2  ':'
#define HORZ1  '-'
#define HORZ2  '='

#define DISPLAY_TITLE "| Display |"
#define EDIT_TITLE "| Edit |"
#define QUIT_TITLE "| F1->Quit |"

void draw_borders(WINDOW * screen, char horiz, char vert, char corner)
{
	int x, y, i;

	getmaxyx(screen, y, x);

	// 4 corners
	mvwaddch(screen, 0, 0, corner);
	mvwaddch(screen, y - 1, 0, corner);
	mvwaddch(screen, 0, x - 1, corner);
	mvwaddch(screen, y - 1, x - 1, corner);

	// sides
	for(i = 1; i < (y - 1); i++) {
		mvwaddch(screen, i, 0, vert);
		mvwaddch(screen, i, x - 1, vert);
	}

	// top and bottom
	for(i = 1; i < (x - 1); i++) {
		mvwaddch(screen, 0, i, horiz);
		mvwaddch(screen, y - 1, i, horiz);
	}
}

#define BUFLEN 1024
int main(int argc, char *argv[])
{
	int parent_x, parent_y;
	int edit_size = 3;

	initscr();
	noecho();
	cbreak();
	keypad(stdscr,TRUE);
	curs_set(FALSE);

	// always do this at startup, otherwise you have strange initial
	// refresh semantics
	wclear(stdscr);
	refresh();

	// set up initial windows
	getmaxyx(stdscr, parent_y, parent_x);

	WINDOW *display = newwin(parent_y - edit_size, parent_x, 0, 0);
	draw_borders(display, HORZ1, VERT1, CORNER);
	mvwprintw(display, 0, 3, DISPLAY_TITLE);
	mvwprintw(display, 0, parent_x-3-strlen(QUIT_TITLE), QUIT_TITLE);
	wrefresh(display);

	WINDOW *edit = newwin(edit_size, parent_x, parent_y - edit_size, 0);
	draw_borders(edit, HORZ2, VERT2, CORNER);
	mvwprintw(edit, 0, 3, EDIT_TITLE);
	wrefresh(edit);


	int d = 0;
	char buf[BUFLEN];
	int ch;
	while((ch = getch()) != KEY_F(1)) {
		switch (ch) {
			case KEY_LEFT:
				strncpy( buf, "KEY_LEFT", BUFLEN );
				mvwprintw( display, d++ + 2, 2, buf );
				d = d % (parent_y - 4 - edit_size);
				wrefresh(display);
				break;
			case KEY_F(2):
				strncpy( buf, "KEY_F(2)", BUFLEN );
				mvwprintw( display, d++ + 2, 2, buf );
				d = d % (parent_y - 4 - edit_size);
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
					d = d % (parent_y - 4 - edit_size);
					wrefresh(display);
				}
				break;
		}
	}

	// close curses lib, reset terminal
	endwin();

	return 0;
}

