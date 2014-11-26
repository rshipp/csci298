#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crr.h"
#include "reservations.h"

/* Reservations */


static int compare_reservations(const void* reservationx, const void* reservationy) {
    int compare_room = strcmp((* (struct Reservation * const *) reservationx)->room, (* (struct Reservation * const *) reservationy)->room);
    if (!compare_room) {
        int compare_start = (* (struct Reservation * const *) reservationx)->start - (* (struct Reservation * const *) reservationy)->start;
        if (!compare_start) {
            return (* (struct Reservation * const *) reservationx)->end - (* (struct Reservation * const *) reservationy)->end;
        } else {
            return compare_start;
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

    /* Return length of array. */
    return i;
}



/*
room_available(room, time) {
    qsort
    if (time < room->start || time >= room->end) {
        return ;
    } else {
        return ;
    }
}


rooms_available(rooms, roomslen, time) {
    int i;
    for (i=0; i<roomslen; i++) {
        if (rooms[i]->start) {

        }
    }
}
*/
