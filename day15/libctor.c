#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "attachable_debugger.h"

static void libctor( void ) __attribute__((constructor));

void libctor( void )
{
	attachable_debugger();
}

