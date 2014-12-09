#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "crr.h"
#include "crrses.h"
#include "handlers.h"

/* REQ3: main application features / user interface elements */

void* newreservation_handler(char** rooms, int roomslen, struct Reservation** sched, int* schedlen, struct Reservation** partial, struct Reservation** list, WINDOW* window, int winheight, char* line) {
    int d = 0;
    cleardisplay(window);

    struct tm* t = calloc(1, sizeof(struct tm));
    if (!t) {
        fputs("Error allocating memory\n", stderr); /* REQ6 */
        return NULL;
    }
    char* tr = strptime(line, "%F %T\n", t);
    if (tr == NULL || tr[0] != '\0') {
        writeline(window, winheight, &d, "Enter a date and 24-hour time in the format: YYYY-MM-DD HH:MM:SS");
        writeline(window, winheight, &d, "Invalid timestamp. Try again.");
        free(t);
        return newreservation_handler;
    }
    time_t time = mktime(t);

    writelinef(window, winheight, &d, "Rooms available on %s", ctime(&time));

    char** available;
    int numavailable = rooms_available(rooms, roomslen, *sched, *schedlen, time, &available);
    if (!numavailable) {
        writeline(window, winheight, &d, "None");
        free(available);
        free(t);
        return newreservation_handler;
    }
    int i;
    for (i=0; i<numavailable; i++) {
        writeline(window, winheight, &d, available[i]);
        free(available[i]);
    }
    free(available);

    writeline(window, winheight, &d, "");
    writeline(window, winheight, &d, "Choose a room.");

    free(t);
    return nr_pickaroom_handler;
}

void* nr_pickaroom_handler(char** rooms, int roomslen, struct Reservation** sched, int* schedlen, struct Reservation** partial, struct Reservation** list, WINDOW* window, int winheight, char* line) {
    int d = 0;
    cleardisplay(window);

    int i;
    for (i=0; i<roomslen; i++) {
        if (!strcmp(line, rooms[i])) {
            strncpy((*partial)->room, line, sizeof((*partial)->room));
            writelinef(window, winheight, &d, "Reserving %s", line);
            writeline(window, winheight, &d, "Enter a starting time in YYYY-MM-DD HH:MM:SS format.");
            return nr_start_handler;
        }
    }

    writeline(window, winheight, &d, "Invalid room. Try again.");
    return nr_pickaroom_handler;
}

void* nr_start_handler(char** rooms, int roomslen, struct Reservation** sched, int* schedlen, struct Reservation** partial, struct Reservation** list, WINDOW* window, int winheight, char* line) {
    int d = 0;
    cleardisplay(window);

    struct tm* t = calloc(1, sizeof(struct tm));
    if (!t) {
        fputs("Error allocating memory\n", stderr); /* REQ6 */
        return NULL;
    }
    char* tr = strptime(line, "%F %T\n", t);
    if (tr == NULL || tr[0] != '\0') {
        writeline(window, winheight, &d, "Enter a starting time in the format: YYYY-MM-DD HH:MM:SS");
        writeline(window, winheight, &d, "Invalid timestamp. Try again.");
        free(t);
        return nr_start_handler;
    }
    time_t time = mktime(t);

    (*partial)->start = time;
    writelinef(window, winheight, &d, "Reserving %s", (*partial)->room);
    writelinef(window, winheight, &d, "Start time %s", ctime(&time));
    writeline(window, winheight, &d, "Enter an ending time (AFTER the starting time) in YYYY-MM-DD HH:MM:SS format.");

    free(t);
    return nr_end_handler;
}

void* nr_end_handler(char** rooms, int roomslen, struct Reservation** sched, int* schedlen, struct Reservation** partial, struct Reservation** list, WINDOW* window, int winheight, char* line) {
    int d = 0;
    cleardisplay(window);

    struct tm* t = calloc(1, sizeof(struct tm));
    if (!t) {
        fputs("Error allocating memory\n", stderr); /* REQ6 */
        return NULL;
    }
    char* tr = strptime(line, "%F %T\n", t);
    if (tr == NULL || tr[0] != '\0') {
        writeline(window, winheight, &d, "Enter an ending time (AFTER the starting time) in YYYY-MM-DD HH:MM:SS format.");
        writeline(window, winheight, &d, "Invalid timestamp. Try again.");
        free(t);
        return nr_end_handler;
    }
    time_t time = mktime(t);
    if (time <= (*partial)->start) {
        writeline(window, winheight, &d, "Enter an ending time (AFTER the starting time) in YYYY-MM-DD HH:MM:SS format.");
        writeline(window, winheight, &d, "Must be AFTER! Try again.");
        free(t);
        return nr_end_handler;
    }

    (*partial)->end = time;
    writelinef(window, winheight, &d, "Reserving %s", (*partial)->room);
    writelinef(window, winheight, &d, "Start time %s", ctime(&(*partial)->start));
    writelinef(window, winheight, &d, "End time %s", ctime(&time));
    writeline(window, winheight, &d, "Enter a description, <128 chars.");

    free(t);
    return nr_desc_handler;
}

void* nr_desc_handler(char** rooms, int roomslen, struct Reservation** sched, int* schedlen, struct Reservation** partial, struct Reservation** list, WINDOW* window, int winheight, char* line) {
    int d = 0;
    cleardisplay(window);

    strncpy((*partial)->description, line, sizeof((*partial)->description));
    if (!reservation_add(sched, schedlen, *partial)) {
        writeline(window, winheight, &d, "The reservation conflicts with an existing one in your schedule.");
        writeline(window, winheight, &d, "It will be discarded.");
        writeline(window, winheight, &d, "Press Enter to return to the main menu.");
        return main_handler;
    }
    writeline(window, winheight, &d, "The room has been reserved.");
    writeline(window, winheight, &d, "Press Enter to return to the main menu.");

    return main_handler;
}

void* dayview_handler(char** rooms, int roomslen, struct Reservation** sched, int* schedlen, struct Reservation** partial, struct Reservation** list, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    struct tm* t = calloc(1, sizeof(struct tm));
    if (!t) {
        fputs("Error allocating memory\n", stderr); /* REQ6 */
        return NULL;
    }
    char* tr = strptime(line, "%F\n", t);
    if (tr == NULL || tr[0] != '\0') {
        writeline(window, winheight, &d, "Enter a date in the format: YYYY-MM-DD");
        writeline(window, winheight, &d, "Invalid date. Try again.");
        free(t);
        return dayview_handler;
    }
    time_t time = mktime(t);

    writelinef(window, winheight, &d, "Reservations on %s", line);

    struct Reservation* reservations;
    int numreservations = reservations_for_day(time, *sched, *schedlen, &reservations);
    if (!numreservations) {
        writeline(window, winheight, &d, "None");
        free(t);
        return dayview_handler;
    }
    int i;
    for (i=0; i<numreservations; i++) {
        snprintf( buf, BUFSIZE, "%d) %s", i, reservations[i].room );
        mvwprintw( window, d++ + 2, 2, buf );
        d = d % winheight;
        writelinef(window, winheight, &d, "   %s", ctime(&(reservations[i].start)));
        writelinef(window, winheight, &d, "   %s", ctime(&(reservations[i].end)));
    }

    writeline(window, winheight, &d, "");
    writeline(window, winheight, &d, "Choose a number to view or edit a reservation.");
    free(*list);
    *list = reservations;

    free(t);
    return resview_handler;
}

void* roomview_handler(char** rooms, int roomslen, struct Reservation** sched, int* schedlen, struct Reservation** partial, struct Reservation** list, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    writelinef(window, winheight, &d, "Reservations for %s", line);

    struct Reservation* reservations;
    int numreservations = reservations_for_room(line, *sched, *schedlen, &reservations);
    if (!numreservations) {
        writeline(window, winheight, &d, "None");
        free(reservations);
        return roomview_handler;
    }
    int i;
    for (i=0; i<numreservations; i++) {
        snprintf( buf, BUFSIZE, "%d) %s", i, reservations[i].room );
        mvwprintw( window, d++ + 2, 2, buf );
        d = d % winheight;
        writelinef(window, winheight, &d, "   %s", ctime(&(reservations[i].start)));
        writelinef(window, winheight, &d, "   %s", ctime(&(reservations[i].end)));
    }

    writeline(window, winheight, &d, "");
    writeline(window, winheight, &d, "Choose a number to view or edit a reservation.");
    free(*list);
    *list = reservations;

    return resview_handler;
}

void* resview_handler(char** rooms, int roomslen, struct Reservation** sched, int* schedlen, struct Reservation** partial, struct Reservation** list, WINDOW* window, int winheight, char* line) {
    int d = 0;
    cleardisplay(window);

    int index = atoi(line);
    writeline(window, winheight, &d, "Reservation details");
    writeline(window, winheight, &d, "");
    writelinef(window, winheight, &d, "Room: %s", ((*list)[index]).room);
    writelinef(window, winheight, &d, "Desc: %s", ((*list)[index]).description);
    writelinef(window, winheight, &d, "Start: %s", ctime(&((*list)[index]).start));
    writelinef(window, winheight, &d, "End: %s", ctime(&((*list)[index]).end));
    writeline(window, winheight, &d, "");
    writeline(window, winheight, &d, "Leave blank and press Enter twice to return to the main menu, or");
    writeline(window, winheight, &d, "enter 'd' and press Enter to delete this reservation, or");
    writeline(window, winheight, &d, "enter a date and 24-hour time in YYYY-MM-DD HH:MM:SS format to edit.");

    memcpy(*partial, &(*list)[index], sizeof(struct Reservation));

    return edit_handler;
}

void* edit_handler(char** rooms, int roomslen, struct Reservation** sched, int* schedlen, struct Reservation** partial, struct Reservation** list, WINDOW* window, int winheight, char* line) {
    int d = 0;
    cleardisplay(window);

    if (strncmp("d\n", line, 2) == 0) {
        reservation_delete(sched, schedlen, *partial);
        return main_handler;
    } else if (strncmp("\n", line, 1) == 0) {
        return main_handler;
    } else {
        /* delete the reservation */
        reservation_delete(sched, schedlen, *partial);

        struct tm* t = calloc(1, sizeof(struct tm));
        if (!t) {
            fputs("Error allocating memory\n", stderr); /* REQ6 */
            return NULL;
        }
        char* tr = strptime(line, "%F %T\n", t);
        if (tr == NULL || tr[0] != '\0') {
            writeline(window, winheight, &d, "Enter a date and 24-hour time in the format: YYYY-MM-DD HH:MM:SS");
            writeline(window, winheight, &d, "Invalid timestamp. Try again.");
            free(t);
            return newreservation_handler;
        }
        time_t time = mktime(t);

        writelinef(window, winheight, &d, "Rooms available on %s", ctime(&time));

        char** available;
        int numavailable = rooms_available(rooms, roomslen, *sched, *schedlen, time, &available);
        if (!numavailable) {
            writeline(window, winheight, &d, "None");
            free(t);
            return newreservation_handler;
        }
        int i;
        for (i=0; i<numavailable; i++) {
            writeline(window, winheight, &d, available[i]);
        }

        writeline(window, winheight, &d, "");
        writeline(window, winheight, &d, "Choose a room.");

        free(t);
        return nr_pickaroom_handler;
    }
}

void* search_handler(char** rooms, int roomslen, struct Reservation** sched, int* schedlen, struct Reservation** partial, struct Reservation** list, WINDOW* window, int winheight, char* line) {
    char buf[BUFSIZE];
    int d = 0;
    cleardisplay(window);

    /* remove trailing newline */
    line[strlen(line)-1] = '\0';
    writelinef(window, winheight, &d, "Reservations matching '%s'", line);

    struct Reservation* reservations;
    int numreservations = reservations_search(line, *sched, *schedlen, &reservations);
    if (!numreservations) {
        writeline(window, winheight, &d, "None");
        writeline(window, winheight, &d, "");
        writeline(window, winheight, &d, "Enter a string to search for:");
        free(reservations);
        return search_handler;
    }
    int i;
    for (i=0; i<numreservations; i++) {
        snprintf( buf, BUFSIZE, "%d) %s", i, reservations[i].room );
        mvwprintw( window, d++ + 2, 2, buf );
        d = d % winheight;
        writelinef(window, winheight, &d, "   %s", reservations[i].description);
        writelinef(window, winheight, &d, "   %s", ctime(&(reservations[i].start)));
        writelinef(window, winheight, &d, "   %s", ctime(&(reservations[i].end)));
    }

    writeline(window, winheight, &d, "");
    writeline(window, winheight, &d, "Choose a number to view or edit a reservation.");
    free(*list);
    *list = reservations;

    return resview_handler;
}

void* main_handler(char** rooms, int roomslen, struct Reservation** sched, int* schedlen, struct Reservation** partial, struct Reservation** list, WINDOW* window, int winheight, char* line) {
    int d = 0;
    cleardisplay(window);
    switch((int)line[0]) {
        case (int)'1':
            writeline(window, winheight, &d, "Enter a date and 24-hour time in the format: YYYY-MM-DD HH:MM:SS");
            return newreservation_handler;
        case (int)'2':
            writeline(window, winheight, &d, "Enter a date in the format: YYYY-MM-DD");
            return dayview_handler;
        case (int)'3':
            writeline(window, winheight, &d, "Enter a room name:");
            for (int i=0; i<roomslen; i++) {
                writeline(window, winheight, &d, rooms[i]);
            }
            return roomview_handler;
        case (int)'4':
            writeline(window, winheight, &d, "Enter a string to search for:");
            return search_handler;
        default:
            writeline(window, winheight, &d, "Invalid choice. Try again.");
            writeline(window, winheight, &d, "Select an option:");
            writeline(window, winheight, &d, "1) Make a new reservation");
            writeline(window, winheight, &d, "2) View/edit reservations for a day");
            writeline(window, winheight, &d, "3) View/edit reservations for a room");
            writeline(window, winheight, &d, "4) Search and edit/delete reservations");
            return main_handler;
    }
    return NULL;
}
