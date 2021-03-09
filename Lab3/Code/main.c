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

/*
void pivot(int row1, int row2)
{
    int i;
    // iterating through columns

    # pragma omp parallel num_threads(num_threads)
    {
        # pragma omp for
        for(i = 0; i < size + 1; i++)
        {
            double temp = matrix[row1][i];
            matrix[row1][i] = matrix[row2][i];
            matrix[row2][i] = temp;
        }
    }
}

int find_max_row(int col)
{
    int i;
    double max = 0;
    int max_ind = -1;
    // iterating through rows
    for(i = col; i < size; i++)
    {
        if( fabs(matrix[i][col]) > fabs(max) )
        {
            max = matrix[i][col];
            max_ind = i;
        }
    }
    return max_ind;
}


void reduce(int cur)
{
    int row = cur + 1;
    for(row; row< size; row++)
    {
        double factor = matrix[cur][cur]/matrix[row][cur];
        int col;
        for(col = cur; col < size + 1; col++)
        {
            matrix[row][col] *= factor;
            matrix[row][col] -= matrix[cur][col];
        }
    }
}

void get_solution()
{
    vector = CreateVec(size);
    int i;
    for(i = 0; i < size; i++)
    {
        vector[i] = matrix[i][size] / matrix[i][i];
    }
}
*/

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
        // # pragma omp for
        
        // # pragma omp parallel for num_threads(thread_count) shared(k, matrix)
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
                // printf("%d\n", omp_get_num_threads());
                matrix[i][j] = matrix[i][j] - factor * matrix[k][j];
            }
        }
    }
    //jordan
    for(k = size - 1; k > 0; k--)
    {
        // # pragma omp parallel for num_threads(thread_count)
        for(i = 0; i < k; i++)
        {
            matrix[i][size] = matrix[i][size] - (matrix[i][k] / matrix[k][k]) * matrix[k][size];
            matrix[i][k] = 0;
        }
    }
    // # pragma omp parallel for num_threads(thread_count)
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
    // printf("%d", size);
    
    // for(i = 0; i < size; i++)
    // {
    //     for(j = 0; j < size + 1; j++)
    //     {
    //         printf("%f ", matrix[i][j]);
    //     }
    //     printf("\n");
    // }

    // printf("\n");

    GET_TIME(start);
    
    solve();
    GET_TIME(end);
    
    // for(i = 0; i < size; i++)
    // {
    //     for(j = 0; j < size + 1; j++)
    //     {
    //         printf("%f ", matrix[i][j]);
    //     }
    //     printf("\n");
    // }

    // get_solution();

    // for(j = 0; j < size; j++)
    // {
    //     printf("%f ", vector[j]);
    // }
    // printf("\n");
    printf("%f\n", end-start);
    
    Lab3SaveOutput(vector, size, end-start);
    
    return 0;
}