/***
 * time accelerator for roomtempd testing
 */

#include <sys/types.h> // open
#include <sys/stat.h>
#include <sys/fcntl.h>

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <poll.h>
#include <time.h>
#include <dlfcn.h>  // for hijacking

#include "rtsys.h"
#include "timeaccel.h"

static int thermfd = -1;
static time_t curtime = 0;
static time_t (*systime)( time_t* );
static int (*syspoll)( struct pollfd*, nfds_t, int );

static void init_time_accel( void )
{
	thermfd = open( TIMEACCFIFO, O_WRONLY|O_NONBLOCK );
	if( thermfd<0 ) {
		fprintf( stderr, "timeaccel:  could not open '%s'\n", TIMEACCFIFO );
		_exit(1);
	}
	systime = dlsym(RTLD_NEXT, "time" );
	syspoll = dlsym(RTLD_NEXT, "poll" );
}

time_t time( time_t* pt )
{
	if( !curtime ) {
		fputs( "timeaccel:  time is being accelerated\n", stderr );
		curtime = systime( pt );
		return curtime;
	} 
	// subsequent calls to curtime 
	return curtime;
}

int poll( struct pollfd* pfds, nfds_t N, int timeout )
{
	if( timeout > 0 ) {
		// send immediately with zero timeout to make sure there is not pending activity
		int pr = syspoll( pfds, N, 0 );
		if( pr ) {
			// activity!
			return pr;
		}

		// some sort of pause for readable debug scrolling
		sleep(3);

		// artifically bump time --- timeout value is in msec!
		curtime += timeout / 1000 + 1;

		// send the new accelerated time to the thermostat
		struct timeaccel_t msg;
		msg.type = 0; // _binary_ time 
		msg.data.newt = curtime;
		// we set a errno for diagnostics
		if( write( thermfd, &msg, sizeof(msg) ) != sizeof(msg) ) {
			fprintf( stderr, "timeaccel: error writing accleration data to thermostat via '%s'\n", TIMEACCFIFO );
			errno = EPIPE;
			return -1;
		}
		// return as if the time has truely elapsed
		return 0;
	} 

	/***
	 * no timeout? ok if you say so
	 * --- but we keep track of how long we've been in the kernel
	 */
	time_t s = systime(NULL);
	int pr = syspoll( pfds, N, timeout );
	time_t f = systime(NULL);
	curtime += f - s;
	return pr;
}

