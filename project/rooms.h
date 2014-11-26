#ifndef CRR_ROOMS
#define CRR_ROOMS

#include <stdio.h>

/* Room */
static int compare_rooms(const void* roomx, const void* roomy);

/* Rooms */
int readrooms(FILE* fp, char*** rooms);

#endif
