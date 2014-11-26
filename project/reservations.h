#ifndef CRR_RESERVATIONS
#define CRR_RESERVATIONS

#include <time.h>

/* Reservations */
struct Reservation {
    char room[50];
    char description[128];
    time_t start;
    time_t end;
};

struct Reservation* makeemptyreservation();
struct Reservation* makereservation(char* room, char* description, time_t start, time_t end);
static int compare_reservations(const void* reservationx, const void* reservationy);
struct Reservation* readreservation(FILE* fp);
int writereservation(FILE* fp, struct Reservation* r);

/* Schedules */
int writesched(FILE* fp, struct Reservation** sched, int schedsize);
int readsched(FILE* fp, struct Reservation*** sched);
int room_available(char* room, struct Reservation** sched, int schedlen, time_t time);
int rooms_available(char** rooms, int roomslen, struct Reservation** sched, int schedlen, time_t time, char*** available);

#endif
