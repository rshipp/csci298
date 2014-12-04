/***
 * room temperature model
 */

#ifndef TMODEL_H
#define TMODEL_H

#include <time.h>

struct tmodel_t {
	double curtemp;
	char thstate;   // state of temperature control 
	time_t last;    // time of last interaction
	double hrate;   // linear model slopes for heating and cooling
	double crate;
};

void setup_tmodel( void );
void init_random_tmodel( struct tmodel_t* const tm, time_t now );
const char* const update_tmodel( struct tmodel_t* const tm, time_t now, char newstate );

#endif // TMODEL_H
