/***
 * The thermodstat "back-end" for crr
 */

#include <sys/types.h> // mkfifo
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/signalfd.h>
#include <signal.h>

#include <errno.h>
#include <poll.h>

#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

#include "rtsys.h"
#include "rooms.h"
#include "tmodel.h"
#include "timeaccel.h"

/***
 * globals
 */
int rooms = 0;
struct room_t* roomarr = NULL;
int* roomfds = NULL;
int respfd = -1;   // response fd
int taccfd = -1;  // time acc fifo
struct tmodel_t* rmtemps = NULL;

void shutdown( void ) 
{
	char fifoname[NAME_MAX];
	if( rooms ) {
		for( int i=0; i<rooms; i++ ) {
			if( roomfds && roomfds[i] >= 0 ) {
				close( roomfds[i] );
				// ignore failure --- other end might still be open
				unlink( fifoname_from_room( fifoname, roomarr[i].name ) );
			}
		}
		free( roomarr );
		if( roomfds ) {
			free( roomfds );
		}
	}
	
	if( respfd >= 0 ) {
		close( respfd );
	}
	unlink( RESPONSEFIFO );

	if( taccfd >= 0 ) {
		close( taccfd );
	}
	unlink( TIMEACCFIFO );

	if( rmtemps ) {
		free( rmtemps );
	}

}

int setup_fifo( const char* fifoname, const char* const descr, int iomode ) 
{
	int fd;
	mode_t fm = 0666;
	if( mkfifo( fifoname, fm ) ) {
		if( errno == EEXIST ) {
			//fprintf( stderr, "warning: %s pre-exists, a common artifact of debugging so we continue.\n", fifoname );
			;
		} else {
			perror( fifoname );
			fprintf( stderr, "error: could not create %s fifo (%s)\n", descr, fifoname );
			return -1;
		}
	}
	if( (fd = open(fifoname, O_NONBLOCK|iomode )) < 0 ) {
		perror( fifoname );
		fprintf( stderr, "error: could not open %s fifo\n", descr );
		return -1;
	}
	return fd;
}

int main( int argc, char* argv[] )
{
	/***
	 * read rooms, create fifos, bail on any failure with a descriptive message
	 */
	if( argc < 2 ) {
		fputs( "usage: $ ./thermodstat rooms.dat\n", stderr );
		exit(1);
	}

	/***
	 * prepare to exit
	 */
	atexit( shutdown );

	/***
	 * create room fifos
	 */
	roomarr = read_rooms( argv[1], &rooms );
	if( !roomarr ) {
		fprintf( stderr, "error: unable to allocate room array or parse '%s'\n", argv[1] );
		exit(1);
	}

	roomfds = calloc( sizeof( int ), rooms );
	if( !roomfds ) {
		perror( "allocating room fds array" );
		exit(1);
	}
	memset( roomfds, 0xff, rooms );   // 2s comp -1

	char fifoname[NAME_MAX];
	for( int i=0; i<rooms; i++ ) {
		fifoname_from_room( fifoname, roomarr[i].name );
		if(( roomfds[i]=setup_fifo( fifoname, roomarr[i].name, O_RDONLY )) < 0 ) {
			exit(1);
		}
	}

	/***
	 * response and time acc fifo
	 */
	if(( respfd = setup_fifo( RESPONSEFIFO, "response", O_RDWR )) < 0 ) {
		exit(1);
	}
	if(( taccfd = setup_fifo( TIMEACCFIFO, "time accel", O_RDONLY )) < 0 ) {
		exit(1);
	}


	/***
	 * Set up a signal set for the signals we are interested in handling
	 */
	sigset_t daemon_sigs;
	sigemptyset( &daemon_sigs );
	sigaddset( &daemon_sigs, SIGINT );

	sigprocmask( SIG_BLOCK, &daemon_sigs, NULL );
	int sigfd = signalfd( -1, &daemon_sigs, SFD_NONBLOCK );
	if( sigfd < 0 ) {
		perror( "signalfd" );
		exit(1);
	}

	/***
	 * initialize room models
	 */
	setup_tmodel();
	rmtemps = calloc( sizeof(struct tmodel_t), rooms );
	if( !rmtemps ) {
		perror( "allocating room temperature models" );
		exit(1);
	}
	time_t now = time(NULL);
	for( int i=0; i<rooms; i++ ) {
		init_random_tmodel( &rmtemps[i], now );
	}

	/***
	 * initialize polling interface
	 */
	const int TACCFD = rooms;
	const int SIGNALFD = rooms + 1;
	const int POLLSIZE = rooms + 2;
	struct pollfd* pfds = calloc( sizeof( struct pollfd ), POLLSIZE );
	// initialize the room pdfs
	for( int i=0; i<rooms; i++ ) {
		pfds[i].fd = roomfds[i];	
		pfds[i].events = POLLIN;   // watch for input
		pfds[i].revents = 0;
	}
	// the time accel fifo
	pfds[TACCFD].fd = taccfd;
	pfds[TACCFD].events = POLLIN;   // watch for input
	pfds[TACCFD].revents = 0;
	// signals
	pfds[SIGNALFD].fd =sigfd;
	pfds[SIGNALFD].events = POLLIN;   // watch for input
	pfds[SIGNALFD].revents = 0;

	/**
	 * okay, watch descriptors
	 */
	const int TIMEOUT=-1;
	int time_acceled = 0;
	int pr;
	do {
		pr = poll(pfds, POLLSIZE, TIMEOUT);
		// errno set
		if( pr < 0 ) {
			perror( "poll" );
			continue;
		}

		// look for signal (we simply clean up as best we can
		if( pfds[SIGNALFD].revents ) {
			fputs( "received signal, exiting 'gracefully'\n", stderr );
			// calls shutdown via atexit(3)
			exit(0);
		}

		// look for time accel data --- ALWAYS DO THIS FIRST, otherwise we 
		// would report "old" temps!
		if( pfds[TACCFD].revents ) {
			struct timeaccel_t tacc;
			int r = read( pfds[TACCFD].fd, &tacc, sizeof(tacc) );
			if( r < 0 ) {
				perror( "read taccfd" );
			} else if( r == 0 ) {
				/***
				 * ignore short reads, I'm skeptical to recieve one, as long as I don't corrupt
				 * my now, all is OK
				 */
				; 
			} else if( tacc.type == '+' ) {
				// "textual" offset by shell for debugging, + proceeded by the num of minutes 
				int minutes;
				tacc.data.text[r-2] = '\0';  // terminate for parsing, -2 because r count the type byte
				if( (sscanf( (char*)&tacc, "%d", &minutes )==1 ) && (minutes>0)) {
					now += 60*minutes;
					time_acceled = 1;
					fprintf( stderr, "msg: time accelerated %d secs via text offset\n", minutes*60 );
				}
			} else {
				// this is the way the time accelerator sends data
				now = tacc.data.newt;
				time_acceled = 1;
			}
			pr--;
			pfds[TACCFD].revents = 0;
		}

		// look for room fifo
		for( int i=0; i<rooms && pr>0; i++ ) {
			if( pfds[i].revents ) {
				if( pfds[i].revents & POLLIN ) {
					// data to read 
					char msg[FIFOMSGLEN];
					int r = read( pfds[i].fd, msg, FIFOMSGLEN );
					if( r < 0 ) {
						perror( "read room fifo" );
						fprintf( stderr, "warning: zero read for room '%s'\n", roomarr[i].name );
					} else if( r == 0 ) {
						fprintf( stderr, "warning: short read for room '%s' (zero bytes)\n", roomarr[i].name );
					} else {
						int showmsg = 1;
						if( r == FIFOMSGLEN ) {
							switch( msg[0] ) {
								case HEAT : case COOL : case QUERY : case OFF :
									fprintf( stderr, "msg: read for room '%s': %c", roomarr[i].name, msg[0] );
									showmsg = 0;
									if( !time_acceled ) time( &now );
									const char* const curtmp = update_tmodel( &rmtemps[i], now, 
											msg[0]==QUERY ? rmtemps[i].thstate : msg[0] );
									char response[FIFOMSGLEN];
									memset( response, ' ', FIFOMSGLEN );
									response[FIFOMSGLEN-1] = '\0';
									response[0] = curtmp[0];
									response[1] = curtmp[1];
									response[2] = curtmp[2];
									response[4] = msg[0];
									response[6] = 'O';
									response[7] = 'K';
									// write the response, ignore errors
									write( respfd, response, FIFOMSGLEN );
									break;
								default :
									fprintf( stderr, "error: read for room '%s': ", roomarr[i].name );
									break;
								}
						} else {
							fprintf( stderr, "error: short read for room (%d bytes) '%s': ", r, roomarr[i].name );
						}
						for( int c=0; showmsg && c<r; c++ ) {
							if( isprint(msg[c]) ) {
								fputc( msg[c], stderr );
							} else {
								fprintf( stderr, "<%02x>", msg[c] );
							}
						}
						fputs( "\n", stderr );  // terminate warning message
					}
				} else {
					// closed connection -- reinit fd
					close( pfds[i].fd );
					pfds[i].fd = open( fifoname_from_room( fifoname, roomarr[i].name ), O_RDONLY|O_NONBLOCK );
					if( pfds[i].fd < 0 ) {
						fprintf( stderr, "warning: poll event %d for room '%s' (disabled room)\n", pfds[i].revents, roomarr[i].name );
						pfds[i].fd = taccfd;
						pfds[i].events *= -1;
					} 
				}

				pfds[i].revents = 0;
				pr--;
			}
		}

	} while( 1 );

	exit( 0 );
}

