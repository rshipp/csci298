#ifndef TIMEACCEL_H
#define TIMEACCEL_H

struct timeaccel_t {
	char type;   // '+' for shell, _binary_ 0 for shared lib
	union {
		time_t newt;
		char   text[sizeof(time_t)];
	} data;
};

static void init_time_accel( void ) __attribute__(( constructor ));

#endif // TIMEACCEL_H
