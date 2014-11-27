#ifndef CRR_HANDLERS
#define CRR_HANDLERS

#include <ncurses.h>

#include "reservations.h"

void* end_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line);
void* nr_pickaroom_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line);
void* nr_start_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line);
void* nr_end_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line);
void* nr_desc_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line);
void* newreservation_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line);
void* dayview_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line);
void* resview_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line);
void* roomview_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line);
void* edit_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line);
void* search_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line);
void* main_handler(char** rooms, int roomslen, struct Reservation*** sched, int* schedlen, struct Reservation** partial, struct Reservation*** list, WINDOW* window, int winheight, char* line);

#endif
