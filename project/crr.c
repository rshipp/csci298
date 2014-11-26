#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <ctype.h>
#include <unistd.h>

#include "crr.h"
#include "crrses.h"
#include "handlers.h"
#include "reservations.h"
#include "rooms.h"


/* Main */

int main(int argc, char* argv[])
{
    if (argc < 3) {
        fputs("Not enough arguments\n", stderr);
        return 1;
    }

    /* Read in rooms data. */
    FILE* roomsfile = fopen(argv[1], "r");
    if (!roomsfile) {
        fprintf(stderr, "Error opening file '%s' for reading\n", argv[1]);
        return 1;
    }
    char** rooms;
    int roomslen = readrooms(roomsfile, &rooms);
    if (!rooms) {
        return 1;
    }

#ifdef CRR_WRITESCHED
    close(schedfile);
    FILE* wschedfile = fopen(argv[2], "w");
    if (!wschedfile) {
        fprintf(stderr, "Error opening file '%s' for reading\n", argv[2]);
        return 1;
    }
    struct Reservation* wsched = malloc(sizeof(struct Reservation));
    int schedsize = 1;
    fwrite(&schedsize, sizeof(int), 1, wschedfile);
    strncpy(wsched->room, "Roomname\n", sizeof(wsched->room));
    strncpy(wsched->description, "Desc ript ion.\n", sizeof(wsched->description));
    wsched->start = 12;
    wsched->end = 52;
    writereservation(wschedfile, wsched);
#endif


    /* Read in schedule data, if it exists. */
    FILE* schedfile = fopen(argv[2], "r");
    struct Reservation** sched;
    int schedlen = 0;
    if (schedfile) {
        schedlen = readsched(schedfile, &sched);
        if (!sched) {
            return 1;
        }
    }

    /* curses */
	initscr();
	noecho();
	cbreak();
	keypad(stdscr,TRUE);
	curs_set(FALSE);

	// set up initial windows
	WINDOW* display = newwin(1, 1, 0, 0 );
	WINDOW* edit = newwin(1,1, 0, 0 );
	int dispheight = size_display( display, edit );

	int d = 0;
    int e = 0;
	char buf[BUFSIZE];
    char line[BUFSIZE];
    int ch;
    void* (*inputhandler)(char**, int, struct Reservation**, int, WINDOW*, int, char*);

    writeline(display, dispheight, &d, buf, "Select an option:");
    writeline(display, dispheight, &d, buf, "1) Make a new reservation");
    writeline(display, dispheight, &d, buf, "2) View/edit reservations for a day");
    writeline(display, dispheight, &d, buf, "3) View/edit reservations for a room");
    writeline(display, dispheight, &d, buf, "4) Search and edit/delete reservations");
    inputhandler = main_handler;

	while((ch = getch()) != KEY_ESC) {
		switch (ch) {
			case KEY_RESIZE:
				strncpy( buf, "KEY_RESIZE", BUFSIZE );
				mvwprintw( display, d++ + 2, 2, buf );
				d = d % dispheight;
				dispheight = size_display( display, edit );
				break;
			default :
				if ( isprint(ch) ) {
                    line[e] = (char)ch;
					snprintf( buf, BUFSIZE, "%c", ch );
					mvwprintw( edit, 1, ++e + 1, buf );
					wrefresh(edit);
                } else if ( ch == KEY_LF || ch == KEY_ENTER ) {
                    line[e] = '\0';
                    e = 0;
                    wclear(edit);
	                draw_borders(edit, HORZ2, VERT2, CORNER);
	                mvwprintw(edit, 0, 3, EDIT_TITLE);
                    wrefresh(edit);
                    // Handle the input.
                    inputhandler = inputhandler(rooms, roomslen, sched, schedlen, display, dispheight, line);
                    if (!inputhandler) {
	                    endwin();
                        return 1;
                    }
                } else if ( ch == KEY_DEL || ch == KEY_BACKSPACE ) {
					snprintf( buf, BUFSIZE, "%c", ' ' );
					mvwprintw( edit, 1, e + 1, buf );
                    line[--e] = '\0';
					wrefresh(edit);
				}
				break;
		}
    }

	// close curses lib, reset terminal
	endwin();

    return 0;
}


