#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <ctype.h>
#include <unistd.h>

#include "crr.h"

#define CORNER '+'
#define VERT1  '|'
#define VERT2  ':'
#define HORZ1  '-'
#define HORZ2  '='

#define DISPLAY_TITLE "| crr |"
#define EDIT_TITLE "| prompt |"
#define QUIT_TITLE "| Esc->Quit |"
/**
 * For some reason, ncurses' KEY_ENTER is defined as 0x157, which
 * corresponds to SHIFT+ENTER on my machine. Using the linefeed character
 * instead seems to work. KEY_BACKSPACE is wrong too, so I'm using DEL.
 * KEY_ESC is not defined at all.
 */
#define KEY_LF 0x0a
#define KEY_DEL 0x7f
#define KEY_ESC 0x1b


#define BUFSIZE 1024
#define MAXROOMLEN 50

/* Stolen from charcell-resize.c */
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

void writelinef(WINDOW* window, int winheight, int* c, char* buf, char* format, char* line) {
    snprintf( buf, BUFSIZE, format, line );
    mvwprintw( window, (*c)++ + 2, 2, buf );
    *c = *c % winheight;
    wrefresh(window);
}
void writeline(WINDOW* window, int winheight, int* c, char* buf, char* line) {
    writelinef(window, winheight, c, buf, "%s", line);
}

void cleardisplay(WINDOW* window) {
	int parent_x, parent_y;
	getmaxyx(stdscr, parent_y, parent_x);
	wclear(window);
	draw_borders(window, HORZ1, VERT1, CORNER);
	mvwprintw(window, 0, 3, DISPLAY_TITLE);
	mvwprintw(window, 0, parent_x-3-strlen(QUIT_TITLE), QUIT_TITLE);
}

/* move these later */
void* end_handler(WINDOW* window, int winheight, char* line) {
    return NULL;
}

void* newreservation_handler(WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    struct tm* t = malloc(sizeof(struct tm));
    if (!strptime(line, "%F %T", t)) {
        writeline(window, winheight, &d, buf, "Enter a date and 24-hour time in the format: YYYY-MM-DD HH:MM:SS");
        writeline(window, winheight, &d, buf, "Invalid timestamp. Try again.");
        return newreservation_handler;
    }
    time_t time = mktime(t);

    writelinef(window, winheight, &d, buf, "Rooms available on %s", ctime(&time));
    return end_handler;
}

void* dayview_handler(WINDOW* window, int winheight, char* line) {
    return NULL;
}

void* roomview_handler(WINDOW* window, int winheight, char* line) {
    return NULL;
}

void* search_handler(WINDOW* window, int winheight, char* line) {
    return NULL;
}

void* main_handler(WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);
    switch((int)line[0]) {
        case (int)'1':
            writeline(window, winheight, &d, buf, "Enter a date and 24-hour time in the format: YYYY-MM-DD HH:MM:SS");
            return newreservation_handler;
        case (int)'2':
            return dayview_handler;
        case (int)'3':
            return roomview_handler;
        case (int)'4':
            return search_handler;
        default:
            writeline(window, winheight, &d, buf, "Invalid choice. Try again.");
            writeline(window, winheight, &d, buf, "Select an option:");
            writeline(window, winheight, &d, buf, "1) Make a new reservation");
            writeline(window, winheight, &d, buf, "2) View/edit reservations for a day");
            writeline(window, winheight, &d, buf, "3) View/edit reservations for a room");
            writeline(window, winheight, &d, buf, "4) Search and edit/delete reservations");
            return main_handler;
    }
    return NULL;
}

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
    void* (*inputhandler)(WINDOW*, int, char*);

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
                    inputhandler = inputhandler(display, dispheight, line);
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

int readrooms(FILE* fp, char*** rooms) {
    (*rooms) = malloc(sizeof(char*)*BUFSIZE);
    if (!(*rooms)) {
        fputs("Error allocating memory\n", stderr);
        return 0;
    }
    int n;
    for (n=0; n<BUFSIZE; n++) {
        (*rooms)[n] = malloc(sizeof(char)*MAXROOMLEN);
        if (!(*rooms)[n]) {
            fputs("Error allocating memory\n", stderr);
            return 0;
        }
    }
    int r = 1, i = 0;
    while (fgets((*rooms)[i], MAXROOMLEN, fp)) {
        if (i>=BUFSIZE-1) {
            r++;
            (*rooms) = realloc((*rooms), sizeof(char*)*BUFSIZE*r);
            if (!(*rooms)) {
                fputs("Error allocating memory\n", stderr);
                return 0;
            }
            for (n=BUFSIZE*(r-1); n<BUFSIZE*r; n++) {
                (*rooms)[n] = malloc(sizeof(char)*MAXROOMLEN);
                if (!(*rooms)[n]) {
                    fputs("Error allocating memory\n", stderr);
                    return 0;
                }
            }
        }
        if (strcmp((*rooms)[i], "\n")) {
            //printf("%s", (*rooms)[i]);
            ++i;
        }
    }

    /* Return array length. */
    return i;
}

struct Reservation* readreservation(FILE* fp) {
    struct Reservation* r = malloc(sizeof(struct Reservation));
    if (!r) {
        fputs("Error allocating memory\n", stderr);
        return NULL;
    }
    if(!fgets(r->room, sizeof(r->room), fp)) {
        fputs("Error reading data1\n", stderr);
        return NULL;
    }
    if(!fgets(r->description, sizeof(r->description), fp)) {
        fputs("Error reading data2\n", stderr);
        return NULL;
    }
    if (fread(&r->start, sizeof(r->start), 1, fp) != 1) {
        fputs("Error reading data3\n", stderr);
        return NULL;
    }
    if (fread(&r->end, sizeof(r->end), 1, fp) != 1) {
        fputs("Error reading data4\n", stderr);
        return NULL;
    }

    return r;
}

int writereservation(FILE* fp, struct Reservation* r) {
    if(!fputs(r->room, fp)) {
        fputs("Error writing data\n", stderr);
        return 0;
    }
    if(!fputs(r->description, fp)) {
        fputs("Error writing data\n", stderr);
        return 0;
    }
    if (fwrite(&r->start, sizeof(r->start), 1, fp) != 1) {
        fputs("Error writing data\n", stderr);
        return 0;
    }
    if (fwrite(&r->end, sizeof(r->end), 1, fp) != 1) {
        fputs("Error writing data\n", stderr);
        return 0;
    }

    return 1;
}

int readsched(FILE* fp, struct Reservation*** sched) {
    (*sched) = malloc(sizeof(struct Reservation*)*BUFSIZE);
    if (!(*sched)) {
        fputs("Error allocating memory\n", stderr);
        return 0;
    }
    int schedsize;
    if (fread(&schedsize, sizeof(int), 1, fp) != 1) {
        fputs("Error reading data\n", stderr);
        return 0;
    }
    int i, n;
    int r = 1;
    for (i=0; i<=schedsize-1; i++) {
        (*sched)[i] = malloc(sizeof(struct Reservation)*MAXROOMLEN);
        if (!(*sched)[i]) {
            fputs("Error allocating memory\n", stderr);
            return 0;
        }
        (*sched)[i] = readreservation(fp);
        if (!(*sched)[i]) {
            return 0;
        }
        if (i>=(BUFSIZE-1)*r) {
            r++;
            (*sched) = realloc((*sched), sizeof(struct Reservation*)*BUFSIZE*r);
            if (!(*sched)) {
                fputs("Error allocating memory\n", stderr);
                return 0;
            }
            for (n=BUFSIZE*(r-1); n<BUFSIZE*r; n++) {
                (*sched)[n] = malloc(sizeof(struct Reservation)*MAXROOMLEN);
                if (!(*sched)[n]) {
                    fputs("Error allocating memory\n", stderr);
                    return 0;
                }
            }
        }
    }

    /* Return length of array. */
    return i;
}
