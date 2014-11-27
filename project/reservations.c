#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crr.h"
#include "reservations.h"

/* Reservations */

struct Reservation* makeemptyreservation() {
    struct Reservation* reservation = malloc(sizeof(struct Reservation));
    strncpy(reservation->room, "", sizeof(reservation->room));
    strncpy(reservation->description, "", sizeof(reservation->description));
    reservation->start = 0;
    reservation->end = 0;
    return reservation;
}

struct Reservation* makereservation(char* room, char* description, time_t start, time_t end) {
    struct Reservation* reservation = malloc(sizeof(struct Reservation));
    strncpy(reservation->room, room, sizeof(reservation->room));
    strncpy(reservation->description, description, sizeof(reservation->description));
    reservation->start = start;
    reservation->end = end;
    return reservation;
}

static int compare_reservations(const void* reservationx, const void* reservationy) {
    int compare_room = strcmp((* (struct Reservation * const *) reservationx)->room, (* (struct Reservation * const *) reservationy)->room);
    if (!compare_room) {
        /* Rooms are the same.
         * If ystart >= xend, x < y.
         * If yend <= xstart, x > y.
         * Else x overlaps y and there is a conflict.
         */

        if ((* (struct Reservation * const *) reservationy)->start >= (* (struct Reservation * const *) reservationx)->end) {
            return -1;
        } else if ((* (struct Reservation * const *) reservationy)->end <= (* (struct Reservation * const *) reservationx)->start) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return compare_room;
    }
}

struct Reservation* readreservation(FILE* fp) {
    struct Reservation* r = malloc(sizeof(struct Reservation));
    if (!r) {
        fputs("Error allocating memory\n", stderr);
        return NULL;
    }
    if(!fgets(r->room, sizeof(r->room), fp)) {
        fputs("Error reading data\n", stderr);
        return NULL;
    }
    if(!fgets(r->description, sizeof(r->description), fp)) {
        fputs("Error reading data\n", stderr);
        return NULL;
    }
    if (fread(&r->start, sizeof(r->start), 1, fp) != 1) {
        fputs("Error reading data\n", stderr);
        return NULL;
    }
    if (fread(&r->end, sizeof(r->end), 1, fp) != 1) {
        fputs("Error reading data\n", stderr);
        return NULL;
    }

    return r;
}

int writereservation(FILE* fp, struct Reservation* r) {
    if(!fputs(r->room, fp)) {
        fputs("Error writing data\n", stderr);
        return 0;
    }
    char* description = r->description;
    if (r->description[strlen(r->description)-1] != '\n') {
        description = strncat(r->description, "\n", 1);
    }
    if(!fputs(description, fp)) {
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


/* Schedules */

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

    sched_modified = 1;
    /* Return length of array. */
    return i;
}

int writesched(FILE* fp, struct Reservation** sched, int schedsize) {
    if (fwrite(&schedsize, sizeof(int), 1, fp) != 1) {
        fputs("Error writing data\n", stderr);
        return 0;
    }
    int i;
    for (i=0; i<schedsize; i++) {
        if (!writereservation(fp, sched[i])) {
            return 0;
        }
    }

    /* Return reservations written. */
    return i;
}


int room_available(char* room, struct Reservation** sched, int schedlen, time_t time) {
    int i;
    for (i=0; i<schedlen; i++) {
        if (strcmp(room, sched[i]->room)) {
            if (time < sched[i]->start || time >= sched[i]->end) {
                continue;
            } else {
                return 0;
            }
        }
    }
    return 1;
}


int rooms_available(char** rooms, int roomslen, struct Reservation** sched, int schedlen, time_t time, char*** available) {
    *available = malloc(sizeof(char*)*roomslen);
    if (!(*available)) {
        fputs("Error allocating memory\n", stderr);
        return 0;
    }
    int i, n = 0;
    for (i=0; i<roomslen; i++) {
        if (room_available(rooms[i], sched, schedlen, time)) {
            (*available)[n] = malloc(MAXROOMLEN);
            if (!(*available)[n]) {
                fputs("Error allocating memory\n", stderr);
                return 0;
            }
            strncpy((*available)[n], rooms[i], MAXROOMLEN);
            ++n;
        }
    }

    /* Return length of array. */
    return n;
}

int reservations_for_room(char* room, struct Reservation** sched, int schedlen, struct Reservation*** reservations) {
    *reservations = malloc(sizeof(struct Reservation*)*schedlen);
    if (!(*reservations)) {
        fputs("Error allocating memory\n", stderr);
        return 0;
    }
    int i, n = 0;
    for (i=0; i<schedlen; i++) {
        if (!strcmp(room, sched[i]->room)) {
            (*reservations)[n] = sched[i];
            ++n;
        }
    }

    return n;
}

int reservations_for_day(time_t time, struct Reservation** sched, int schedlen, struct Reservation*** reservations) {
    *reservations = malloc(sizeof(struct Reservation*)*schedlen);
    if (!(*reservations)) {
        fputs("Error allocating memory\n", stderr);
        return 0;
    }

    int day = time/(24*60*60);

    int i, n = 0;
    for (i=0; i<schedlen; i++) {
        if (day == (sched[i]->start)/(24*60*60) || day == (sched[i]->end)/(24*60*60)) {
            (*reservations)[n] = sched[i];
            ++n;
        }
    }

    return n;
}

int reservations_search(char* needle, struct Reservation** sched, int schedlen, struct Reservation*** reservations) {
    *reservations = malloc(sizeof(struct Reservation*)*schedlen);
    if (!(*reservations)) {
        fputs("Error allocating memory\n", stderr);
        return 0;
    }

    int i, n = 0;
    for (i=0; i<schedlen; i++) {
        if (strcasestr(sched[i]->description, needle)) {
            (*reservations)[n] = sched[i];
            ++n;
        }
    }

    return n;
}
