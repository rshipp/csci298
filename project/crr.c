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
    char room[MAXROOMLEN];
    char (* rooms)[MAXROOMLEN] = malloc(sizeof(room)*BUFSIZE);
    int i = 0;
    while (fgets(room[i], MAXROOMLEN, roomsfile)) {
        if (strcmp(room[i], "\n")) {
            //rooms[i] = room;
            printf("%s", room[i]);
            ++i;
        }
    }
}
