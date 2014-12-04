#ifndef ROOMS_H
#define ROOMS_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define ROOMNAME 49  // wiki: 48 character names
struct room_t {
	char name[ROOMNAME];
};

struct room_t* read_rooms( const char roomdat[], int* rooms_count );

/***
 * Should be NAME_MAX length at fifoname
 */
char* fifoname_from_room( char fifoname[], const char roomname[] );


#endif // ROOMS_H
