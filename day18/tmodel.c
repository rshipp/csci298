/***
 * room temperature model
 */
#include <assert.h>
#include <stdio.h>

#include "rngs.h"
#include "rvgs.h"

#include "rtsys.h"
#include "tmodel.h"

static double empty_lo;
static double empty_hi;
static double onedeg;  
static double fivedeg;

static double f2c( double f )
{
	return (f-32.)*5./9;
}

void setup_tmodel( void )
{
	static int seeds[] = { 1021, 474, 3345, 7383, 537, 58373, 953 };
	int s = time(NULL)%(sizeof(seeds)/sizeof(int));

	PutSeed(seeds[s]);
	empty_lo = f2c(EMPTY_LO);
	empty_hi = f2c(EMPTY_HI);
	onedeg = f2c(1);
	fivedeg = f2c(5);
}


static double random_temp( void )
{
	double u = Random();
	if( u < 0.10 ) {
		return Uniform( empty_lo-fivedeg, empty_hi+fivedeg );
	} else if( u < 0.20 ) {
		return Uniform( empty_hi-fivedeg, empty_hi+fivedeg );
	} 
	return Uniform( empty_lo+onedeg, empty_hi-onedeg );
}

static char random_thstate( void )
{
	double u = Random();
	if( u < 0.80 ) {
		return OFF;
	} else if( u < 0.90 ) {
		return HEAT;
	} 
	return COOL;
}

static double random_rate( void )
{
	const double maxrate = 0.05*((f2c(110)-f2c(100))/(90*60));
	return Uniform( maxrate*0.3, maxrate );
}


void init_random_tmodel( struct tmodel_t* const tm, time_t now )
{
	assert(tm);
	tm->last = now;
	tm->curtemp = random_temp();
	tm->thstate = random_thstate();
	tm->hrate = random_rate();
	tm->crate = -random_rate();
}

#define CDEGLEN 4
char CDEG[CDEGLEN] = {0,};
/** 
 * return the pointer to CDEG which is always updated
 */
const char* const update_tmodel( struct tmodel_t* const tm, time_t now, char newstate )
{
	assert( tm );
	assert( now >= tm->last );
	assert( newstate==HEAT || newstate==COOL || newstate==OFF );

	// update current temperature
	double newtemp = tm->curtemp;
	if( tm->thstate == HEAT ) {
		newtemp = newtemp + tm->hrate*(now-tm->last);
	} else if( tm->thstate == COOL ) {
		newtemp = newtemp + tm->crate*(now-tm->last);
	}

	// threshold
	if( newtemp > 100 ) {
		newtemp = 100;
	} else if( newtemp < 0 ) {
		newtemp = 0;
	}

	//printf( "\nnew %lf old %lf hrate %lf crate %lf op %c %lus",
	//		newtemp, tm->curtemp, tm->hrate, tm->crate, newstate, now-tm->last );
	// set
	tm->curtemp = newtemp;

	// format
	int decitemp = newtemp * 10;
	snprintf( CDEG, CDEGLEN, "%03d", decitemp );
	//printf( " CDEG %s\n", CDEG );

	// update state
	tm->thstate = newstate;

	return CDEG;
}

