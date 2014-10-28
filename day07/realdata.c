#include <stdio.h>
#include <stdlib.h>

#define SIZE 1<<20

int main(int argc, char* argv[])
{
    FILE* fp = fopen("realdata.lis", "r");
    if (!fp) {
        fputs("Error opening file.\n", stderr);
    }
    double* data = malloc(sizeof(double)*SIZE);
    if (!data) {
        fputs("Error allocating memory.\n", stderr);
    }

    for (int i=0; i<SIZE; i++) {
        if (fscanf(fp, "%lf\n", &data[i]) != 1) {
            break;
        }
    }

    free(data);
    fclose(fp);
    return 0;
}
