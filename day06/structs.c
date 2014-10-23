#include <stdio.h>
#include <string.h>

#include "structs.h"

struct book makebook( char* title, char* author, int pubyear )
{
    struct book newbook;
    strncpy( newbook.title, title, BOOKBUF );
    strncpy( newbook.author, author, BOOKBUF );
    newbook.pubyear = pubyear;
    return newbook;
}

struct book* remakebook( struct book* oldbook, char* title, char* author, int pubyear )
{
    strncpy( oldbook->title, title, BOOKBUF );
    strncpy( oldbook->author, author, BOOKBUF );
    oldbook->pubyear = pubyear;
    return oldbook;
}

void set_book_pubyear( struct book* book, int newyear )
{
    if( newyear > 0 ) {
        book->pubyear = newyear;
    }
}

struct movie makemovie( char* name, char* director, int relyear, float theranking )
{
    struct movie newmovie;
    strncpy( newmovie.name, name, BOOKBUF );
    strncpy( newmovie.director, director, BOOKBUF );
    newmovie.relyear = relyear;
    newmovie.theranking = theranking;
    return newmovie;
}

struct song makesong( char* name, char* artist, int relyear, float theranking )
{
    struct song newsong;
    strncpy( newsong.name, name, BOOKBUF );
    strncpy( newsong.artist, artist, BOOKBUF );
    newsong.relyear = relyear;
    newsong.theranking = theranking;
    return newsong;
}

int main( int argc, char* argv[] )
{
    #define BOOKSHELF 8

    struct book book_shelf_1[] = { { "The C Language", "K&R", 1969 }, {}, \
        { "C for Dummies", "Gates", 1989 } };
    
    struct book* book_shelf_2 = calloc( BOOKSHELF, sizeof(struct book) );
    if( !book_shelf_2 ) {
        fputs( "help, I've fallen and lost my memory\n", stderr );
        return 1;
    }
    
    struct book* book_shelf_3 = malloc( BOOKSHELF*sizeof(struct book) );
    if( !book_shelf_3 ) {
        fputs( "help, I've fallen and lost my memory\n", stderr );
        return 1;
    }

    struct book* book_shelf_4 = realloc( book_shelf_3, BOOKSHELF*BOOKSHELF*sizeof(struct book) );
    if( !book_shelf_4 ) {
        fputs( "help, I've fallen and lost my memory\n", stderr );
        return 1;
    }

    int i;
    for (i=0; i<sizeof(book_shelf_1)/sizeof(struct book); i++ ) {
        printf( "Book shelf 1, book %i\n", i );
        printf( "title /%s/\n", book_shelf_1[i].title );
        printf( "author /%s/\n", book_shelf_1[i].author );
        printf( "pubyear /%d/\n", book_shelf_1[i].pubyear );
        puts( "" );
    }

    free( book_shelf_2 );
    free( book_shelf_4 );

    return 0;
}
