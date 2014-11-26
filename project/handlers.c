#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "crr.h"
#include "crrses.h"
#include "handlers.h"

void* end_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, WINDOW* window, int winheight, char* line) {
    return NULL;
}

void* newreservation_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, WINDOW* window, int winheight, char* line) {
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

void* nr_pickaroom_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    int i;
    for (i=0; i<roomslen; i++) {
        if (!strcmp(strcat(line, "\n"), rooms[i])) {
            strncpy((*partial)->room, line, sizeof((*partial)->room));
            writelinef(window, winheight, &d, buf, "Reserving %s", line);
            writeline(window, winheight, &d, buf, "Enter a starting time in YYYY-MM-DD HH:MM:SS format.");
            return nr_start_handler;
        }
    }

    writelinef(window, winheight, &d, buf, "'%s'", line);
    writelinef(window, winheight, &d, buf, "'%s'", rooms[i]);
    writeline(window, winheight, &d, buf, "Invalid room. Try again.");
    return nr_pickaroom_handler;
}

void* nr_start_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    struct tm* t = malloc(sizeof(struct tm));
    if (!strptime(line, "%F %T", t)) {
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

void* nr_end_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    struct tm* t = malloc(sizeof(struct tm));
    if (!strptime(line, "%F %T", t)) {
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

void* nr_desc_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    strncpy((*partial)->description, line, sizeof((*partial)->description));
    (*sched)[*schedlen] = *partial;
    (*schedlen)++;
    writeline(window, winheight, &d, buf, "The room has been reserved.");
    writeline(window, winheight, &d, buf, "Press Enter to return to the main menu.");

    return main_handler;
}

void* dayview_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, WINDOW* window, int winheight, char* line) {
    return NULL;
}

void* roomview_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, WINDOW* window, int winheight, char* line) {
    return NULL;
}

void* search_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, WINDOW* window, int winheight, char* line) {
    return NULL;
}

void* main_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, WINDOW* window, int winheight, char* line) {
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
