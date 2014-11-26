#include <stdlib.h>

#include "crr.h"
#include "crrses.h"
#include "handlers.h"

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
