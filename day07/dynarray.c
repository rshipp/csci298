#include <stdio.h>
#include <stdlib.h>

#define SIZE (1<<20)
int main( int argc, char* argv[] ) 
{
	FILE* fp = fopen( "realdata.lis", "r" );
	if( !fp ) {
		fputs( "Error opening data file.", stderr );
		exit( 1 );
	}

	float* data = malloc( sizeof(float)*SIZE );
	if( !data ) {
		fputs( "Error allocating memory.\n", stderr );
		exit( 1 );
	}

	for( int i=0; i<SIZE; i++ ) {
		if( fscanf( fp, "%f", &data[i] ) != 1 ) {
			fprintf( stderr, "Error reading data index %d\n", i );
			exit(1);
		}
	}

	for( int i=0; i<SIZE; i+=SIZE/8 ) {
		printf( "%07d %.8f\n", i, data[i] );
	}


	free( data);
	fclose(fp);
	return 0;
}
