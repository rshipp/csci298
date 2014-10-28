#include <stdio.h>
#include <stdlib.h>

#define SIZE 1<<20

int comparedouble(const void* left, const void* right)
{
    double diff = *(double*)left - *(double*)right;
    if (diff < 0) {
        return -1;
    } else if (diff > 0) {
        return 1;
    }
    return 0;
}


int main(int argc, char* argv[])
{
    FILE* fp = fopen("realdata.lis", "r");
    if (!fp) {
        fputs("Error opening file.\n", stderr);
        fputs("Error reading data.", stderr);
    }
    double* data = malloc(sizeof(double)*SIZE);
    if (!data) {
        fputs("Error allocating memory.\n", stderr);
        fputs("Error reading data.", stderr);
    }

    for (int i=0; i<SIZE; i++) {
        if (fscanf(fp, "%lf\n", &data[i]) != 1) {
            break;
        }
    }

#ifdef RD_WRITE_BIN
    FILE* bp = fopen("realdata.bin", "w");
    fwrite(data, sizeof(double), SIZE, bp);

    fclose(bp);
#endif

    qsort(data, SIZE, sizeof(double), comparedouble);

    if (fread(data, sizeof(double), SIZE, fp) != SIZE) {
        fputs("Error reading data.", stderr);
        return 1;
    }

#define RD_PRINT_SOME
#ifdef RD_PRINT_SOME
	for( int i=0; i<SIZE; i+=SIZE/8 ) {
		printf( "%07d %.8f\n", i, data[i] );
	}
#endif

    free(data);
    fclose(fp);
    return 0;
}
