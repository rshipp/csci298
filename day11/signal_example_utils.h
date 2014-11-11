/***
 * example interfaces to Linux signal handling
 */

#ifndef SIGNAL_EXAMPLE_UTILS_H
#define SIGNAL_EXAMPLE_UTILS_H

#include <stdio.h>

/***
 * prepare module memory and data
 */
void setup_signal_utils();

/*** shutdown */
void teardown_signal_utils();

/*** handle a signal */
void install_handler( int signum );

/*** print signal history */
void print_signal_history(FILE* fp);

#endif
