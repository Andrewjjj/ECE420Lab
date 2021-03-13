// main

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h> 
#include "timer.h"
#include "Lab3IO.h"
#include <omp.h>

double** matrix;
double* vector;
int thread_count;
int size;

void solve()
{
    int i, k, j;
    //Gaussian
    for(k = 0; k < size-1; k++)
    {
        // find max row
        double max = 0;
        int max_ind = -1;
        for(i = k; i < size; i++)
        {
            if( fabs(matrix[i][k]) > max )
            {
                max = fabs(matrix[i][k]);
                max_ind = i;
            }
        }
        // pivot
        for(i = 0; i < size + 1; i++)
        {
            double temp = matrix[k][i];
            matrix[k][i] = matrix[max_ind][i];
            matrix[max_ind][i] = temp;
        }
        
        # pragma omp parallel for num_threads(thread_count)
        for(i = k+1; i < size; i++)
        {
            double factor = matrix[i][k] / matrix[k][k];
            //iterating through columns
            
            # pragma omp parallel for num_threads(thread_count)
            for(j = k; j < size + 1; j++)
            {
                matrix[i][j] = matrix[i][j] - factor * matrix[k][j];
            }
        }
    }
    //jordan
    for(k = size - 1; k > 0; k--)
    {
        double factor = matrix[k][size] / matrix[k][k];

        # pragma omp parallel for num_threads(thread_count)
        for(i = 0; i < k; i++)
        {
            matrix[i][size] = matrix[i][size] - matrix[i][k] * factor;
            matrix[i][k] = 0;
        }
    }
    
    //getting results
    for(i = 0; i < size; i++)
    {
        vector[i] = matrix[i][size] / matrix[i][i];
    }
}

int main (int argc, char *argv[])
{
    double start, end;
    
    if (argc != 2) {
        printf ("Please pass in the number of threads\n");
        return 1;
    }
    thread_count = atoi(argv[1]);
    Lab3LoadInput(&matrix, &size);
    vector = CreateVec(size);

    GET_TIME(start);
    solve();
    GET_TIME(end);
    
    printf("%f\n", end-start);
    
    Lab3SaveOutput(vector, size, end-start);
    
    return 0;
}