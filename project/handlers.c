#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "crr.h"
#include "crrses.h"
#include "handlers.h"

void* newreservation_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    struct tm* t = malloc(sizeof(struct tm));
    if (!strptime(line, "%F %T\n", t)) {
        writeline(window, winheight, &d, buf, "Enter a date and 24-hour time in the format: YYYY-MM-DD HH:MM:SS");
        writeline(window, winheight, &d, buf, "Invalid timestamp. Try again.");
        return newreservation_handler;
    }
    time_t time = mktime(t);

    writelinef(window, winheight, &d, buf, "Rooms available on %s", ctime(&time));

    char** available;
    int numavailable = rooms_available(rooms, roomslen, *sched, *schedlen, time, &available);
    if (!numavailable) {
        writeline(window, winheight, &d, buf, "None");
        return newreservation_handler;
    }
    int i;
    for (i=0; i<numavailable; i++) {
        writeline(window, winheight, &d, buf, available[i]);
    }

    writeline(window, winheight, &d, buf, "");
    writeline(window, winheight, &d, buf, "Choose a room.");

    return nr_pickaroom_handler;
}

void* nr_pickaroom_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    int i;
    for (i=0; i<roomslen; i++) {
        if (!strcmp(line, rooms[i])) {
            strncpy((*partial)->room, line, sizeof((*partial)->room));
            writelinef(window, winheight, &d, buf, "Reserving %s", line);
            writeline(window, winheight, &d, buf, "Enter a starting time in YYYY-MM-DD HH:MM:SS format.");
            return nr_start_handler;
        }
    }

    writeline(window, winheight, &d, buf, "Invalid room. Try again.");
    return nr_pickaroom_handler;
}

void* nr_start_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    struct tm* t = malloc(sizeof(struct tm));
    if (!strptime(line, "%F %T\n", t)) {
        writeline(window, winheight, &d, buf, "Enter a starting time in the format: YYYY-MM-DD HH:MM:SS");
        writeline(window, winheight, &d, buf, "Invalid timestamp. Try again.");
        return nr_start_handler;
    }
    time_t time = mktime(t);

    (*partial)->start = time;
    writelinef(window, winheight, &d, buf, "Reserving %s", (*partial)->room);
    writelinef(window, winheight, &d, buf, "Start time %s", ctime(&time));
    writeline(window, winheight, &d, buf, "Enter an ending time (AFTER the starting time) in YYYY-MM-DD HH:MM:SS format.");

    return nr_end_handler;
}

void* nr_end_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    struct tm* t = malloc(sizeof(struct tm));
    if (!strptime(line, "%F %T\n", t)) {
        writeline(window, winheight, &d, buf, "Enter an ending time (AFTER the starting time) in YYYY-MM-DD HH:MM:SS format.");
        writeline(window, winheight, &d, buf, "Invalid timestamp. Try again.");
        return nr_end_handler;
    }
    time_t time = mktime(t);
    if (time <= (*partial)->start) {
        writeline(window, winheight, &d, buf, "Enter an ending time (AFTER the starting time) in YYYY-MM-DD HH:MM:SS format.");
        writeline(window, winheight, &d, buf, "Must be AFTER! Try again.");
        return nr_end_handler;
    }

    (*partial)->end = time;
    writelinef(window, winheight, &d, buf, "Reserving %s", (*partial)->room);
    writelinef(window, winheight, &d, buf, "Start time %s", ctime(&(*partial)->start));
    writelinef(window, winheight, &d, buf, "End time %s", ctime(&time));
    writeline(window, winheight, &d, buf, "Enter a description, <128 chars.");

    return nr_desc_handler;
}

void* nr_desc_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    strncpy((*partial)->description, line, sizeof((*partial)->description));
    (*sched)[*schedlen] = *partial;
    (*schedlen)++;
    sched_modified = 1;
    writeline(window, winheight, &d, buf, "The room has been reserved.");
    writeline(window, winheight, &d, buf, "Press Enter to return to the main menu.");

    return main_handler;
}

void* dayview_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    struct tm* t = calloc(1, sizeof(struct tm));
    if (!t) {
        fputs("Error allocating memory\n", stderr);
        return NULL;
    }
    if (!strptime(line, "%F\n", t)) {
        writeline(window, winheight, &d, buf, "Enter a date in the format: YYYY-MM-DD");
        writeline(window, winheight, &d, buf, "Invalid date. Try again.");
        return dayview_handler;
    }
    time_t time = mktime(t);

    writelinef(window, winheight, &d, buf, "Reservations on %s", line);

    struct Reservation** reservations;
    int numreservations = reservations_for_day(time, *sched, *schedlen, &reservations);
    if (!numreservations) {
        writeline(window, winheight, &d, buf, "None");
        return dayview_handler;
    }
    int i;
    for (i=0; i<numreservations; i++) {
        snprintf( buf, BUFSIZE, "%d) %s", i, reservations[i]->room );
        mvwprintw( window, d++ + 2, 2, buf );
        d = d % winheight;
        writelinef(window, winheight, &d, buf, "   %s", ctime(&(reservations[i]->start)));
        writelinef(window, winheight, &d, buf, "   %s", ctime(&(reservations[i]->end)));
    }

    writeline(window, winheight, &d, buf, "");
    writeline(window, winheight, &d, buf, "Choose a number to view or edit a reservation.");
    *list = reservations;

    return resview_handler;
}

void* roomview_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    writelinef(window, winheight, &d, buf, "Reservations for %s", line);

    struct Reservation** reservations;
    int numreservations = reservations_for_room(line, *sched, *schedlen, &reservations);
    if (!numreservations) {
        writeline(window, winheight, &d, buf, "None");
        return roomview_handler;
    }
    int i;
    for (i=0; i<numreservations; i++) {
        snprintf( buf, BUFSIZE, "%d) %s", i, reservations[i]->room );
        mvwprintw( window, d++ + 2, 2, buf );
        d = d % winheight;
        writelinef(window, winheight, &d, buf, "   %s", ctime(&(reservations[i]->start)));
        writelinef(window, winheight, &d, buf, "   %s", ctime(&(reservations[i]->end)));
    }

    writeline(window, winheight, &d, buf, "");
    writeline(window, winheight, &d, buf, "Choose a number to view or edit a reservation.");
    *list = reservations;

    return resview_handler;
}

void* resview_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    int index = atoi(line);
    writeline(window, winheight, &d, buf, "Reservation details");
    writeline(window, winheight, &d, buf, "");
    writelinef(window, winheight, &d, buf, "Room: %s", ((*list)[index])->room);
    writelinef(window, winheight, &d, buf, "Desc: %s", ((*list)[index])->description);
    writelinef(window, winheight, &d, buf, "Start: %s", ctime(&((*list)[index])->start));
    writelinef(window, winheight, &d, buf, "End: %s", ctime(&((*list)[index])->end));
    writeline(window, winheight, &d, buf, "");
    writeline(window, winheight, &d, buf, "Leave blank and press Enter twice to edit, or");
    writeline(window, winheight, &d, buf, "write anything else and press Enter twice to return to the main menu.");

    return edit_handler;
}

void* edit_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    if (strncmp("\n", line, 1)) {
        return main_handler;
    } else {
        return newreservation_handler;
    }
}

void* search_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    /* remove trailing newline */
    line[strlen(line)-1] = '\0';
    writelinef(window, winheight, &d, buf, "Reservations matching '%s'", line);

    struct Reservation** reservations;
    int numreservations = reservations_search(line, *sched, *schedlen, &reservations);
    if (!numreservations) {
        writeline(window, winheight, &d, buf, "None");
        return roomview_handler;
    }
    int i;
    for (i=0; i<numreservations; i++) {
        snprintf( buf, BUFSIZE, "%d) %s", i, reservations[i]->room );
        mvwprintw( window, d++ + 2, 2, buf );
        d = d % winheight;
        writelinef(window, winheight, &d, buf, "   %s", reservations[i]->description);
        writelinef(window, winheight, &d, buf, "   %s", ctime(&(reservations[i]->start)));
        writelinef(window, winheight, &d, buf, "   %s", ctime(&(reservations[i]->start)));
    }

    writeline(window, winheight, &d, buf, "");
    writeline(window, winheight, &d, buf, "Choose a number to view or edit a reservation.");
    *list = reservations;

    return resview_handler;
}

void* main_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    int index = atoi(line);
    cleardisplay(window);
    switch((int)line[0]) {
        case (int)'1':
            writeline(window, winheight, &d, buf, "Enter a date and 24-hour time in the format: YYYY-MM-DD HH:MM:SS");
            return newreservation_handler;
        case (int)'2':
            writeline(window, winheight, &d, buf, "Enter a date in the format: YYYY-MM-DD");
            return dayview_handler;
        case (int)'3':
            writeline(window, winheight, &d, buf, "Enter a room name:");
            for (int i=0; i<roomslen; i++) {
                writeline(window, winheight, &d, buf, rooms[i]);
            }
            return roomview_handler;
        case (int)'4':
            writeline(window, winheight, &d, buf, "Enter a string to search for:");
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
