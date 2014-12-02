/***
 * daymonx.c demonstrates how to setup an application running in daemon(2) mode.
 * 
 */

#include <sys/signalfd.h>
#include <signal.h>
#include <errno.h>
#include <poll.h>
#include <unistd.h>
#include <stdlib.h>  

#include <stdio.h>  // included to simply demonstrate that the standard files get
                    // closed with daemon(2)

#include <string.h>   // for strerror

/** global */
FILE* logfp = NULL;
int sigfd;

void shutdown_daemon( void ) 
{
	// just in case
	if( logfp ) {
		fprintf( logfp, "daymonx[%d] shutdown\n", getpid() );
		fclose( logfp );
	}

	close( sigfd );
	return;
}

void stdfiles_gibberish()
{
	int c;
	/** throw some bits in and out of the standard files
	 */
	fprintf( logfp, "daymonx[%d] %s\n", getpid(), __FUNCTION__ );
	fprintf( stderr, "This is stdout, waiting for keystroke on stdin\n" );
	fprintf( stderr, "This is stderr, waiting for keystroke on stdin\n" );
	c = getchar();
	fprintf( stderr, "Woot, received keystroke x%02x\n", c );
	fprintf( logfp, "daymonx[%d] getchar: received keystroke x%02x\n", getpid(), c );
}


#define POLLARRAY 1    // size of polling array
int main()
{
	int r;  // generic return values 

	logfp = fopen( "daymonx.log", "a" );
	if( !logfp ) {
		perror( "opening daymonx.log" );
		exit( 1 );
	}
	fprintf( logfp, "daymonx[%d] starts\n", getpid() );
	fprintf( stderr, "daymonx[%d] starts\n", getpid() );

	/***
	 * oh, so that's why we have atexit...
	 */
	atexit( shutdown_daemon );

	stdfiles_gibberish();

	/***
	 * Set up a signal set for the signals we are interested in handling
	 */
	sigset_t daemon_sigs;
	sigemptyset( &daemon_sigs );
	sigaddset( &daemon_sigs, SIGUSR1 );
	sigaddset( &daemon_sigs, SIGHUP );
	sigaddset( &daemon_sigs, SIGINT );
	sigaddset( &daemon_sigs, SIGQUIT );
	sigaddset( &daemon_sigs, SIGTERM );

	sigprocmask( SIG_BLOCK, &daemon_sigs, NULL );
	sigfd = signalfd( -1, &daemon_sigs, SFD_NONBLOCK );
	struct signalfd_siginfo siginfo;

	/* poll(2) */
	struct pollfd pollarray[POLLARRAY];   // a real daemon would make this bigger 

	pollarray[0].fd = sigfd;
	pollarray[0].events = POLLIN;         // data available to read (a signal received)
	pollarray[0].revents = 0;       

	do {

		fflush(logfp);  // a convenient place to flush log data

		/***
		 * three different cases:  >0 number of ready fds, <0 error, 0=timeout
		 */
		int readyfds;
		switch( readyfds = poll( pollarray, POLLARRAY, -1 ) ) {
			case 0 :
				// timeout
				fprintf( logfp, "daymonx[%d] poll timeout, continuing\n", getpid() );
				break;

			case -1 :
				if( errno==EINTR ) {
					// a signal that we aren't setup to handle arrived, we ignore it.
					// the user (admin) should know how to use kill well enough to administer
					// the proper signal, and the really important signals
					// (KILL, STOP, SEGV, ...) we can't do anything about anyhow.
					fprintf( logfp, "daymonx[%d] poll (%s), please use SIGUSR1, SIGHUP, or SIGTERM\n", 
							getpid(), strerror(errno) );
				} else {
					/** provide a descriptive message */
					fprintf( logfp, "daymonx[%d] poll (%s), continuing\n", getpid(), strerror(errno) );
				}
				break;
			
			default:
				fprintf( logfp, "daymonx[%d] poll returned %d fd ready\n", getpid(), readyfds );
				// ready file descriptors, readyfds could be used to prevent us
				// from overscanning a list and wasting time, in this case we have just one
				if( pollarray[0].revents & POLLIN ) {
					// readable data for signals
					// NOTE --- the need to use a "raw" low-level read(3) on the file descriptor,
					// .fd is NOT a FILE buffered interface returned by fopen!
					if((r=read( pollarray[0].fd, &siginfo, sizeof(siginfo) )) > 0 ) {
						// handle signal
						fprintf( logfp, "daymonx[%d] signal %s received\n", getpid(), strsignal(siginfo.ssi_signo) );
						switch( siginfo.ssi_signo ) {
							case SIGTERM :
							case SIGINT  :
							case SIGQUIT :
								// clean up with atexit
								exit(0);
								// does not return
								break;
						}
					}else if( r < 0 ) {
						fprintf( logfp, "daymonx read[%d] %s, continuing\n", getpid(), strerror(errno) );
					}
				}
				break;
		}

	} while( !(pollarray[0].revents & POLLERR) );

	fputs( "daymonx loop exit! signal loop failed\n", logfp );
	return(1);
}

