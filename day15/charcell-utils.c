/***
 * charcell utilities for several in lecture code examples.
 */

#include <ncurses.h>
#include <string.h>

#include "charcell-utils.h"

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

int size_display( WINDOW* display, WINDOW* edit )
{
	int parent_x, parent_y;
	int edit_size = 3;

	// always do this at startup or resize, otherwise you have strange initial
	// refresh semantics
	wclear(stdscr);
	refresh();

	// set up initial windows
	getmaxyx(stdscr, parent_y, parent_x);

	wresize(display, parent_y - edit_size, parent_x);
	mvwin(display, 0, 0);
	wclear(display);
	draw_borders(display, HORZ1, VERT1, CORNER);
	mvwprintw(display, 0, 3, DISPLAY_TITLE);
	mvwprintw(display, 0, parent_x-3-strlen(QUIT_TITLE), QUIT_TITLE);

	wresize(edit, edit_size, parent_x);
	mvwin(edit, parent_y-edit_size, 0);
	wclear(edit);
	draw_borders(edit, HORZ2, VERT2, CORNER);
	mvwprintw(edit, 0, 3, EDIT_TITLE);

	wrefresh(display);
	wrefresh(edit);

	return parent_y - 4 - edit_size;
}


