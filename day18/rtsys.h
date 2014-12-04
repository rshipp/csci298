
#ifndef RTSYS_H
#define RTSYS_H

#define RESPONSEFIFO "thermostat.fifo"
#define TIMEACCFIFO  "timeacc.fifo"

#define HEAT 'H'       // WIKI
#define COOL 'C'
#define OFF  'F'
#define QUERY 'Q'

// Degree FAHR 
#define OCCUPIED_HI 74 // WIKI
#define OCCUPIED_LO 68
#define EMPTY_HI    92
#define EMPTY_LO    50

#define FIFOMSGLEN  12 // WIKI

#endif // RTSYS_H
