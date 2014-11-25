#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crr.h"

#define BUFSIZE 1024
#define MAXROOMLEN 50

int main(int argc, char* argv[])
{
    if (argc < 3) {
        fputs("Not enough arguments\n", stderr);
        return 1;
    }

    FILE* roomsfile = fopen(argv[1], "r");
    if (!roomsfile) {
        fprintf(stderr, "Error opening file '%s' for reading\n", argv[1]);
        return 1;
    }
    FILE* schedfile = fopen(argv[2], "r");
    if (!schedfile) {
        fprintf(stderr, "Error opening file '%s' for reading\n", argv[2]);
        return 1;
    }

    /* Read in rooms data. */
    char** rooms = readrooms(roomsfile);
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
    struct Reservation** sched = readsched(schedfile);
    if (!sched) {
        return 1;
    }

    return 0;
}

char** readrooms(FILE* fp) {
    char** rooms = malloc(sizeof(char*)*BUFSIZE);
    if (!rooms) {
        fputs("Error allocating memory\n", stderr);
        return NULL;
    }
    int n;
    for (n=0; n<BUFSIZE; n++) {
        rooms[n] = malloc(sizeof(char)*MAXROOMLEN);
        if (!rooms[n]) {
            fputs("Error allocating memory\n", stderr);
            return NULL;
        }
    }
    int r = 1, i = 0;
    while (fgets(rooms[i], MAXROOMLEN, fp)) {
        if (i>=BUFSIZE-1) {
            r++;
            rooms = realloc(rooms, sizeof(char*)*BUFSIZE*r);
            if (!rooms) {
                fputs("Error allocating memory\n", stderr);
                return NULL;
            }
            for (n=BUFSIZE*(r-1); n<BUFSIZE*r; n++) {
                rooms[n] = malloc(sizeof(char)*MAXROOMLEN);
                if (!rooms[n]) {
                    fputs("Error allocating memory\n", stderr);
                    return NULL;
                }
            }
        }
        if (strcmp(rooms[i], "\n")) {
            //printf("%s", rooms[i]);
            ++i;
        }
    }

    return rooms;
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

struct Reservation* writereservation(FILE* fp, struct Reservation* r) {
    if(!fputs(r->room, fp)) {
        fputs("Error writing data\n", stderr);
        return NULL;
    }
    if(!fputs(r->description, fp)) {
        fputs("Error writing data\n", stderr);
        return NULL;
    }
    if (fwrite(&r->start, sizeof(r->start), 1, fp) != 1) {
        fputs("Error writing data\n", stderr);
        return NULL;
    }
    if (fwrite(&r->end, sizeof(r->end), 1, fp) != 1) {
        fputs("Error writing data\n", stderr);
        return NULL;
    }

    return r;
}

struct Reservation** readsched(FILE* fp) {
    struct Reservation** sched = malloc(sizeof(struct Reservation*)*BUFSIZE);
    if (!sched) {
        fputs("Error allocating memory\n", stderr);
        return NULL;
    }
    int schedsize;
    if (fread(&schedsize, sizeof(int), 1, fp) != 1) {
        fputs("Error reading data\n", stderr);
        return NULL;
    }
    int i, n;
    int r = 1;
    for (i=0; i<=schedsize-1; i++) {
        sched[i] = malloc(sizeof(struct Reservation)*MAXROOMLEN);
        if (!sched[i]) {
            fputs("Error allocating memory\n", stderr);
            return NULL;
        }
        sched[i] = readreservation(fp);
        if (!sched[i]) {
            return NULL;
        }
        if (i>=(BUFSIZE-1)*r) {
            r++;
            sched = realloc(sched, sizeof(struct Reservation*)*BUFSIZE*r);
            if (!sched) {
                fputs("Error allocating memory\n", stderr);
                return NULL;
            }
            for (n=BUFSIZE*(r-1); n<BUFSIZE*r; n++) {
                sched[n] = malloc(sizeof(struct Reservation)*MAXROOMLEN);
                if (!sched[n]) {
                    fputs("Error allocating memory\n", stderr);
                    return NULL;
                }
            }
        }
    }
    for (n=0; n<i; n++) {
        printf("%s%s%d\n%d\n", sched[n]->room, sched[n]->description, sched[n]->start, sched[n]->end);
    }

    return sched;
}
