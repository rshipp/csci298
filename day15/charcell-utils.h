/***
 * charcell utilities for several in lecture code examples.
 */

#ifndef CHARCELL_UTILS_H
#define CHARCELL_UTILS_H

#define CORNER '+'
#define VERT1  '|'
#define VERT2  ':'
#define HORZ1  '-'
#define HORZ2  '='

#define DISPLAY_TITLE "| Display |"
#define EDIT_TITLE "| Edit |"
#define QUIT_TITLE "| F1->Quit |"


void draw_borders(WINDOW * screen, char horiz, char vert, char corner);

int size_display( WINDOW* display, WINDOW* edit );

#endif /* CHARCELL_UTILS_H */
