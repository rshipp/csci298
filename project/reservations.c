#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crr.h"
#include "reservations.h"

/* Reservations */

int compare_reservations(const void* reservationx, const void* reservationy) {
    /* special handling for null reservations, to avoid epoch errors */
    if (strcmp(( (struct Reservation *) reservationx)->room, "") == 0) {
        return -1;
    } else if (strcmp(( (struct Reservation *) reservationy)->room, "") == 0) {
        return 1;
    }
    /* normal reservations */
    int compare_room = strcmp(( (struct Reservation *) reservationx)->room, ( (struct Reservation *) reservationy)->room);
    if (compare_room == 0) {
        /* Rooms are the same.
         * If ystart >= xend, x < y.
         * If yend <= xstart, x > y.
         * Else x overlaps y and there is a conflict.
         */

        if (( (struct Reservation *) reservationy)->start >= ( (struct Reservation *) reservationx)->end) {
            return -1;
        } else if (( (struct Reservation *) reservationy)->end <= ( (struct Reservation *) reservationx)->start) {
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
    if (fread(r, sizeof(struct Reservation), 1, fp) != 1) {
        fputs("Error reading data\n", stderr);
        return NULL;
    }

    return r;
}

int writereservation(FILE* fp, struct Reservation r) {
    if (fwrite(&r, sizeof(r), 1, fp) != 1) {
        fputs("Error writing data\n", stderr);
        return 0;
    }

    return 1;
}


/* Schedules */

int readsched(FILE* fp, struct Reservation** sched) {
    int schedsize;
    int* schedlen = malloc(sizeof(int));
    *schedlen = 0;
    struct Reservation* reservation;
    if (fread(&schedsize, sizeof(int), 1, fp) != 1) {
        fputs("Error reading data\n", stderr);
        return 0;
    }
    (*sched) = malloc(schedsize*sizeof(struct Reservation));
    if (!(*sched)) {
        fputs("Error allocating memory\n", stderr);
        return 0;
    }
    int i;
    for (i=0; i<=schedsize-1; i++) {
        reservation = readreservation(fp);
        if (!reservation) {
            fputs("Error reading data\n", stderr);
            return 0;
        }
        if (!reservation_add(sched, schedlen, reservation)) {
            fputs("Error reading data (schedule conflict)\n", stderr);
            return 0;
        }
    }

    if (!(i == schedsize && schedsize == *schedlen)) {
        fputs("Error reading data (length mismatch)\n", stderr);
        return 0;
    }

    sched_modified = 0;
    /* Return length of array. */
    return i;
}

int writesched(FILE* fp, struct Reservation* sched, int schedsize) {
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


int room_available(char* room, struct Reservation* sched, int schedlen, time_t time) {
    int i;
    for (i=0; i<schedlen; i++) {
        if (!strcmp(room, sched[i].room)) {
            if (time < sched[i].start || time >= sched[i].end) {
                continue;
            } else {
                return 0;
            }
        }
    }
    return 1;
}


int rooms_available(char** rooms, int roomslen, struct Reservation* sched, int schedlen, time_t time, char*** available) {
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

int reservations_for_room(char* room, struct Reservation* sched, int schedlen, struct Reservation** reservations) {
    *reservations = malloc(sizeof(struct Reservation)*schedlen);
    if (!(*reservations)) {
        fputs("Error allocating memory\n", stderr);
        return 0;
    }
    int i, n = 0;
    for (i=0; i<schedlen; i++) {
        if (!strcmp(room, sched[i].room)) {
            (*reservations)[n] = sched[i];
            ++n;
        }
    }

    return n;
}

int reservations_for_day(time_t time, struct Reservation* sched, int schedlen, struct Reservation** reservations) {
    *reservations = malloc(sizeof(struct Reservation)*schedlen);
    if (!(*reservations)) {
        fputs("Error allocating memory\n", stderr);
        return 0;
    }

    int day = time/(24*60*60);

    int i, n = 0;
    for (i=0; i<schedlen; i++) {
        if (day >= (sched[i].start)/(24*60*60) && day <= (sched[i].end)/(24*60*60)) {
            (*reservations)[n] = sched[i];
            ++n;
        }
    }

    return n;
}

int reservations_search(char* needle, struct Reservation* sched, int schedlen, struct Reservation** reservations) {
    *reservations = malloc(sizeof(struct Reservation)*schedlen);
    if (!(*reservations)) {
        fputs("Error allocating memory\n", stderr);
        return 0;
    }

    int i, n = 0;
    for (i=0; i<schedlen; i++) {
        if (strcasestr(sched[i].description, needle)) {
            (*reservations)[n] = sched[i];
            ++n;
        }
    }

    return n;
}

int reservation_add(struct Reservation** sched, int* schedsize, struct Reservation* reservation) {
    for (int i=0; i<(*schedsize); i++) {
        if (compare_reservations(reservation, &(*sched)[i]) == 0) {
            /* can't add overlapping reservations */
            return 0;
        }
    }
    *sched = realloc(*sched, sizeof(struct Reservation)*((*schedsize)+1));
    (*sched)[*schedsize] = *reservation;
    (*schedsize)++;
    qsort(sched, *schedsize, sizeof(struct Reservation), compare_reservations);

    sched_modified = 1;
    return 1;
}

void reservation_delete(struct Reservation** sched, int* schedsize, struct Reservation* reservation) {
    for (int i=0; i<(*schedsize); i++) {
        if (compare_reservations(reservation, &(*sched)[i]) == 0) {
            for(int n=i+1; n<(*schedsize); n++) {
                (*sched)[n-1] = (*sched)[n];
            }
            (*schedsize)--;
            *sched = realloc(*sched, sizeof(struct Reservation)*((*schedsize)));
            break;
        }
    }
    sched_modified = 1;
}
