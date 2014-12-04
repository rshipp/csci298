/***
 * An example client using thermostat.c heating and cooling interface
 * via fifos.
 *
 * See the wiki assignment page
 *   https://cs.mcprogramming.com/lp/Projects/RoomTemp
 * for complete protocol details.
 */

#include <poll.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "rtsys.h"

void prompt( void ) 
{
	puts( "Enter your choice: " );
	fflush(stdout);
}

void show_menu( void ) 
{
	time_t t = time(NULL);
	char tbuf[28];   // need 26 for ctime_r
	ctime_r(&t,tbuf);

	fputs( "*******************************************\n** Time: ", stdout );
	fputs( tbuf, stdout );
	puts( "*******************************************\n" );
	
	puts( "Select an option\n"
"  Q ) query room temperature\n"
"  H ) heat room\n"
"  C ) cool room\n"
"  F ) turn heat and cool off\n"
"  w ) wait\n"
"  x ) quit\n");
	fflush(stdout);
	
}

int main( int argc, char* argv[] ) 
{

	if( argc != 2 ) {
		fputs( "usage: $ example-thermostat-client  room.fifo\n\nroom.fifo should pre-exist,\nrun thermostat in the same directory for virtual thermostat.\n", stderr );
		exit(1);
	}

	FILE* roomfifo = fopen( argv[1], "w" );
	if( !roomfifo ) {
		perror( argv[1] );
		exit(1);
	}
	setbuf( roomfifo, NULL );

	FILE* respfifo = fopen( RESPONSEFIFO, "r" );
	if( !respfifo ) {
		perror( RESPONSEFIFO );
		exit(1);
	}
	setbuf( respfifo, NULL );

	char roommsg[FIFOMSGLEN];
	memset( roommsg, ' ', FIFOMSGLEN );

	// set up poll
	const int MENU = 0;
	const int RESP = 1;
	struct pollfd pfds[2];
	pfds[MENU].fd = fileno( stdin );
	pfds[MENU].events = POLLIN;
	pfds[MENU].revents = 0;

	pfds[RESP].fd = fileno( respfifo );
	pfds[RESP].events = POLLIN;
	pfds[RESP].revents = 0;

	show_menu();
	prompt();
	int loops = 10;
	int timeout = -1;   // wait forever
	do {
		if( --loops == 0 ) {
			loops = 10;
			show_menu();
		}
		int pr = poll( pfds, 2, timeout );
		if( pr < 0 ) {
			perror( "poll" );
			prompt();
			continue;
		}

		if( pr == 0 ) {
			// timeout, automatically query
			roommsg[0] = QUERY;
			fwrite( roommsg, sizeof(char), FIFOMSGLEN, roomfifo );
			continue;
		}
		/***
		 * handle menu input
		 */
		if( pfds[MENU].revents == POLLIN ) {
			int m = getchar();
			switch( m ) {
				case 'x' : exit(0); break;
				case 'w' :
					puts( "Enter the number of minutes to wait: " );
					fflush(stdout);
					int t;
					if( scanf( "%d", &t ) == 1 && t > 0 ) {
						printf( "Waiting %d minutes.\n", t );
						timeout = t*1000*60; // poll uses milliseconds
					} else {
						printf( "Could not read timeout > 0, try again.\n" );
					}
					break;
				case 'Q' :
				case 'H' :
				case 'F' :
				case 'C' :
					// exactly 16 bytes
					roommsg[0] = m;
					fwrite( roommsg, sizeof(char), FIFOMSGLEN, roomfifo );
					break;
			}
			pfds[MENU].revents = 0;
		}
		/***
		 * handle responses from the thermostat
		 */
		if( pfds[RESP].revents == POLLIN ) {
			char msg[FIFOMSGLEN+1];
			if( fread( msg, sizeof(char), FIFOMSGLEN, respfifo ) == FIFOMSGLEN ) {
				msg[FIFOMSGLEN] = '\0';
				printf( "Received thermostat response: %16s\n", msg );
				float cel;
				if( sscanf( msg, "%f ", &cel ) == 1 ) {
					printf( "Room is %.1f degrees Fahr.\n", (cel/10)*9/5+32 );
				}
			} else {
				perror( "read response" );
			}
			pfds[RESP].revents = 0;
		}	

		prompt();

	} while( 1 );

	exit(0);
}
