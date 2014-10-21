
/**
 * usage:  wordat dictionaryname offset
 *
 * Displays the word at offset in the file dictionary name,
 * or nothing if the character at offset is '\n'
 *
 * dictionaries are specially formatted files with exactly one "word"
 * per line.  A word is made up of any and all characters from the
 * beginning of the line to the end of the line.  A line has only one
 * one newline character, the newline character is not part of the 
 * word.
 */

#include <stdio.h>
#include <string.h>

int main( int argc, char* argv[] )
{
	FILE* fp;
	long fpoffset = 0;
	long offset;  /* will be valued by sscanf on argv[2] */
	const int BUFFSIZE = 1024;
	char buffer[BUFFSIZE];
	if( argc < 3 ) {
		fputs( "Not enough args.\n", stderr );
		return 1;
	}

	/** open the command line provided filename */
	if(( fp = fopen( argv[1], "r" )) == NULL ) {
		fprintf( stderr, "Cannot open '%s'\n", argv[1] );	
		return 1;
	}

	/** parse the integer offset provided */
	if(( sscanf( argv[2], "%li", &offset )) != 1 ) {
		fprintf( stderr, "'%s' is an invalid offset.\n", argv[2] );	
		return 1;
	}

	while( fpoffset <= offset ) {
		/** read the next line */
		fgets( buffer, 1024, fp );	

		if( fpoffset + strlen(buffer) <= offset ) {
			/**
			 * we aren't there yet, we must get
			 * PAST the offset to have the word in our buffer... 
			 */
			fpoffset += strlen(buffer);
			continue;
		}
		
		/**
		 * if we are this far, the offset specified on the command line is 
		 * somewhere in this buffer...
		 */

		/* is it the precise offset of the newline character at the end? */
		if( fpoffset + strlen(buffer) - 1 == offset ) {
			/**
			 * yep, that's it.  the description says we should not print
			 * anything in this case.
			 */
			break;
		}

		/* otherwise, because of the formatting of dictionary files, the 
		 * buffer contains the whole word we are looking for...
		 */
		fputs( buffer, stdout );  /* newline included in the buffer */

		/* and thats all we must do */
		break;
	}

	fclose(fp);
	return 0;
}

