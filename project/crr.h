
struct Reservation {
    char room[50];
    char description[128];
    time_t start;
    time_t end;
};

char** readrooms(FILE* fp);
struct Reservation* readreservation(FILE* fp);
int writereservation(FILE* fp, struct Reservation* r);
struct Reservation** readsched(FILE* fp);
