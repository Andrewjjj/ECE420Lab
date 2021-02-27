// main
/*
 gcc -g -Wall -fopenmp -o omp_quickSort1 omp_quickSort1.c -fnested-functions
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "timer.h"
#include "Lab3IO.h"

double** matrix;
int* vector;
int num_threads;
int size;

int main (int argc, char *argv[])
{
    int i, j;
    
    if (argc != 2) {
        printf ("Please pass in the number of threads\n");
        return 1;
    }
    num_threads = atoi(argv[1]);
    Lab3LoadInput(&matrix, &size);
    printf("%d", size);
    
    for(i = 0; i < size; i++)
    {
        for(j = 0; j < size; j++)
        {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    
    
    return 0;
}