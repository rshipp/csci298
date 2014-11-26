#ifndef CRR_CRRSES
#define CRR_CRRSES

#include <ncurses.h>

#define CORNER '+'
#define VERT1  '|'
#define VERT2  ':'
#define HORZ1  '-'
#define HORZ2  '='

#define DISPLAY_TITLE "| crr |"
#define EDIT_TITLE "| prompt |"
#define QUIT_TITLE "| Esc->Quit |"
/**
 * For some reason, ncurses' KEY_ENTER is defined as 0x157, which
 * corresponds to SHIFT+ENTER on my machine. Using the linefeed character
 * instead seems to work. KEY_BACKSPACE is wrong too, so I'm using DEL.
 * KEY_ESC is not defined at all.
 */
#define KEY_LF 0x0a
#define KEY_DEL 0x7f
#define KEY_ESC 0x1b

void draw_borders(WINDOW * screen, char horiz, char vert, char corner);
int size_display( WINDOW* display, WINDOW* edit );
void writelinef(WINDOW* window, int winheight, int* c, char* buf, char* format, char* line);
void writeline(WINDOW* window, int winheight, int* c, char* buf, char* line);
void cleardisplay(WINDOW* window);

#endif
