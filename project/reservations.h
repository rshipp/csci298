#ifndef CRR_RESERVATIONS
#define CRR_RESERVATIONS

#include <time.h>

/* REQ3: Reservations */
struct Reservation {
    char room[50];
    char description[128];
    time_t start;
    time_t end;
};

int compare_reservations(const void* reservationx, const void* reservationy);
struct Reservation* readreservation(FILE* fp);
int writereservation(FILE* fp, struct Reservation r);

/* Schedules */
int writesched(FILE* fp, struct Reservation* sched, int schedsize);
int readsched(FILE* fp, struct Reservation** sched);
int room_available(char* room, struct Reservation* sched, int schedlen, time_t time);
int rooms_available(char** rooms, int roomslen, struct Reservation* sched, int schedlen, time_t time, char*** available);
int reservations_for_room(char* room, struct Reservation* sched, int schedlen, struct Reservation** reservations);
int reservations_for_day(time_t time, struct Reservation* sched, int schedlen, struct Reservation** reservations);
int reservations_search(char* needle, struct Reservation* sched, int schedlen, struct Reservation** reservations);
int reservation_add(struct Reservation** sched, int* schedsize, struct Reservation* reservation);
void reservation_delete(struct Reservation** sched, int* schedsize, struct Reservation* reservation);

#endif
