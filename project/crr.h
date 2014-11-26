
struct Reservation {
    char room[50];
    char description[128];
    time_t start;
    time_t end;
};

int readrooms(FILE* fp, char*** rooms);
struct Reservation* readreservation(FILE* fp);
int writereservation(FILE* fp, struct Reservation* r);
int readsched(FILE* fp, struct Reservation*** sched);
