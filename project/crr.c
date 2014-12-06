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

int sched_modified = 0;

/* Main */

int main(int argc, char* argv[])
{
    prctl( PR_SET_PTRACER, PR_SET_PTRACER_ANY, 0, 0, 0 );
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

    /* Read in schedule data, if it exists. */
    FILE* schedfile = fopen(argv[2], "r");
    struct Reservation* sched;
    int schedlen = 0;
    if (schedfile) {
        schedlen = readsched(schedfile, &sched);
        if (!sched || !schedlen) {
            return 1;
        }
    } else {
        sched = malloc(sizeof(struct Reservation));
    }

    /* curses */
    initscr();
    noecho();
    cbreak();
    keypad(stdscr,TRUE);
    curs_set(FALSE);

    /* set up initial windows */
    WINDOW* display = newwin(1, 1, 0, 0 );
    WINDOW* edit = newwin(1,1, 0, 0 );
    int dispheight = size_display( display, edit );

    int d = 0;
    int e = 0;
    char buf[BUFSIZE];
    char line[BUFSIZE];
    int ch;
    void* (*inputhandler)(char**, int, struct Reservation**, int*, struct Reservation**, struct Reservation**, WINDOW*, int, char*);
    struct Reservation* partial = calloc(1, sizeof(struct Reservation));
    struct Reservation* list;
    int confirmquit = 0;

    writeline(display, dispheight, &d, "Select an option:");
    writeline(display, dispheight, &d, "1) Make a new reservation");
    writeline(display, dispheight, &d, "2) View/edit reservations for a day");
    writeline(display, dispheight, &d, "3) View/edit reservations for a room");
    writeline(display, dispheight, &d, "4) Search and edit/delete reservations");
    inputhandler = main_handler;

    while((ch = getch())) {
        switch (ch) {
            case KEY_RESIZE:
                strncpy( buf, "KEY_RESIZE", BUFSIZE );
                mvwprintw( display, d++ + 2, 2, buf );
                d = d % dispheight;
                dispheight = size_display( display, edit );
                break;
            case KEY_ESC:
                if (!confirmquit && sched_modified) {
                    writeline(display, dispheight, &d, "Press Esc again to save and quit.");
                    confirmquit = 1;
                } else if (!sched_modified) {
                    endwin();
                    free(partial);
                    return 1;
                } else {
                    /* save and quit */
                    if (schedfile) {
                        fclose(schedfile);
                    }
                    FILE* wschedfile = fopen(argv[2], "w");
                    if (!wschedfile) {
                        fprintf(stderr, "Error opening file '%s' for writing\n", argv[2]);
                        free(partial);
                        return 1;
                    }
                    writesched(wschedfile, sched, schedlen);
                    fclose(wschedfile);

                    endwin();
                    free(partial);
                    return 1;
                }
            default :
                if ( isprint(ch) ) {
                    confirmquit = 0;
                    line[e] = (char)ch;
                    snprintf( buf, BUFSIZE, "%c", ch );
                    mvwprintw( edit, 1, ++e + 1, buf );
                    wrefresh(edit);
                } else if ( ch == KEY_LF || ch == KEY_ENTER ) {
                    confirmquit = 0;
                    line[e] = '\0';
                    e = 0;
                    wclear(edit);
                    draw_borders(edit, HORZ2, VERT2, CORNER);
                    mvwprintw(edit, 0, 3, EDIT_TITLE);
                    wrefresh(edit);
                    /* Handle the input. */
                    inputhandler = inputhandler(rooms, roomslen, &sched, &schedlen, &partial, &list, display, dispheight, strncat(line, "\n", 2));
                    if (inputhandler == NULL) {
                        endwin();
                        free(partial);
                        return 1;
                    }
                } else if ( ch == KEY_DEL || ch == KEY_BACKSPACE ) {
                    confirmquit = 0;
                    if (e > 0) {
                        snprintf( buf, BUFSIZE, "%c", ' ' );
                        mvwprintw( edit, 1, e + 1, buf );
                        line[--e] = '\0';
                    }
                    wrefresh(edit);
                }
                break;
        }
    }

    /* close curses lib, reset terminal */
    endwin();

    free(partial);
    return 0;
}


