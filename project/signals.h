#ifndef CRR_SIGNALS
#define CRR_SIGNALS

extern int sighup_received;
extern int sigusr1_received;

void signal_catcher( int signum );
void install_handler( int signum );

#endif
