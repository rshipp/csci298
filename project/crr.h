
struct Reservation {
    char room[50];
    char description[128];
    int start;
    int end;
};

char** readrooms(FILE* fp);
struct Reservation** readsched(FILE* fp);
