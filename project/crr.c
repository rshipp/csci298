#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

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
    FILE* schedfile = fopen(argv[2], "w");
    if (!schedfile) {
        fprintf(stderr, "Error opening file '%s' for writing\n", argv[2]);
        return 1;
    }

    /* Read in rooms data. */
    char** rooms = malloc(sizeof(char*)*BUFSIZE);
    if (!rooms) {
        fputs("Error allocating memory\n", stderr);
        return 1;
    }
    int n;
    for (n=0; n<BUFSIZE; n++) {
        rooms[n] = malloc(sizeof(char)*MAXROOMLEN);
        if (!rooms[n]) {
            fputs("Error allocating memory\n", stderr);
            return 1;
        }
    }
    int r = 1, i = 0;
    while (fgets(rooms[i], MAXROOMLEN, roomsfile)) {
        if (i>=BUFSIZE-1) {
            r++;
            rooms = realloc(rooms, sizeof(char*)*BUFSIZE*r);
            if (!rooms) {
                fputs("Error allocating memory\n", stderr);
                return 1;
            }
            for (n=BUFSIZE*(r-1); n<BUFSIZE*r; n++) {
                rooms[n] = malloc(sizeof(char)*MAXROOMLEN);
                if (!rooms[n]) {
                    fputs("Error allocating memory\n", stderr);
                    return 1;
                }
            }
        }
        if (strcmp(rooms[i], "\n")) {
            //printf("%s", rooms[i]);
            ++i;
        }
    }
}
