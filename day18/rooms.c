#ifndef ROOMS_H

#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "rooms.h"

struct room_t* read_rooms( const char roomdat[], int* rooms_count )
{
	assert( roomdat );
	assert( rooms_count );

	char n[ROOMNAME];
	int c;

	*rooms_count = 0;

	FILE* fp = fopen( roomdat, "r" );
	if( !fp ) {
		perror( roomdat );
		return NULL;
	}

	/** count */
	c = 0;
	while( fgets( n, ROOMNAME, fp ) != NULL ) {
		c++;
	}
	/* allocate */
	struct room_t* rooms = calloc( sizeof(struct room_t), c );
	if( !rooms ) {
		perror( "read_rooms" );
		fclose(fp);
		return NULL;
	}
	/* read into array */
	rewind(fp);
	*rooms_count = 0;
	while( *rooms_count < c && fgets( rooms[*rooms_count].name, ROOMNAME, fp ) != NULL ) {
		char *nl = strchr(rooms[*rooms_count].name,'\n');
		if( nl ) *nl = '\0';
		(*rooms_count)++;
	}
	/* fini */
	fclose(fp);
	return rooms;
}

#define FIFOSUFFIX ".fifo"
char* fifoname_from_room( char fifoname[], const char roomname[] )
{
	int L = strlen(roomname);
	int j = 0;
	assert( L>0 );
	for(int i=0; j<NAME_MAX-1-strlen(FIFOSUFFIX) && i<L; i++ ) {
		if( roomname[i] >= 'a' && roomname[i] <= 'z' ) {
			fifoname[j++] = roomname[i];
		} else if( roomname[i] >= '0' && roomname[i] <= '9' ) {
			fifoname[j++] = roomname[i];
		} else if( roomname[i] >= 'A' && roomname[i] <= 'Z' ) {
			fifoname[j++] = roomname[i] - 'A' + 'a';
		}
	}
	fifoname[j] = '\0';
	assert( j>0 );
	// proper bounds checking in the for loop (given fifoname has NAME_MAX space)
	strcat( fifoname, FIFOSUFFIX );
	return fifoname;
}

#endif
