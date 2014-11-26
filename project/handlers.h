#ifndef CRR_HANDLERS
#define CRR_HANDLERS

#include <ncurses.h>

void* end_handler(WINDOW* window, int winheight, char* line);
void* newreservation_handler(WINDOW* window, int winheight, char* line);
void* dayview_handler(WINDOW* window, int winheight, char* line);
void* roomview_handler(WINDOW* window, int winheight, char* line);
void* search_handler(WINDOW* window, int winheight, char* line);
void* main_handler(WINDOW* window, int winheight, char* line);

#endif
