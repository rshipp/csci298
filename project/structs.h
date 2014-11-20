#ifndef STRUCTS_H
#define STRUCTS_H

#define BOOKBUF 1024

struct book {
    char title[BOOKBUF];
    char author[BOOKBUF];
    int  pubyear;
    float theranking;
};

struct movie {
    char name[BOOKBUF];
    char director[BOOKBUF];
    int  relyear;
    float theranking;
};

struct song {
    char name[BOOKBUF];
    char artist[BOOKBUF];
    int  relyear;
    float theranking;
};

struct book makebook( char* title, char* author, int pubyear );

struct book* remakebook( struct book* oldbook, char* title, char* author, int pubyear );

void set_book_pubyear( struct book* book, int newyear );

struct movie makemovie( char* name, char* director, int relyear, float theranking );

struct song makesong( char* name, char* artist, int relyear, float theranking );

#endif
