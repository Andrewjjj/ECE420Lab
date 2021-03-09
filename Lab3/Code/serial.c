// serial

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "timer.h"
#include "Lab3IO.h"

double** matrix;
double* vector;
int num_threads;
int size;

int pivot(int row1, int row2)
{
    int i;
    // iterating through columns
    for(i = 0; i < size + 1; i++)
    {
        double temp = matrix[row1][i];
        matrix[row1][i] = matrix[row2][i];
        matrix[row2][i] = temp;
    }
}

int find_max_row(int col)
{
    int i;
    double max = 0;
    int max_ind = -1;
    // iterating through rows
    for(i = 0; i < size; i++)
    {
        if( fabs(matrix[i][col]) > fabs(max) )
        {
            max = matrix[i][col];
            max_ind = i;
        }
    }
    return max_ind;
}

/*
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
*/

void solve()
{
    int i, k, j;
    for(k = 0; k < size; k++)
    {
        int row = find_max_row(k);
        pivot(row, k);
        //reduce(k);
        // iterating through rows
        for(i = k+1; i < size; i++)
        {
            double factor = matrix[i][k] / matrix[k][k];
            //iterating through columns
            for(j = k; j < size + 1; j++)
            {
                matrix[i][j] = matrix[i][j] - factor * matrix[k][j];
            }
        }
    }
    for(k = size - 1; k >= 1; k--)
    {
        for(i = 0; i <= k-1; i++)
        {
            matrix[i][size] = matrix[i][size] - (matrix[i][k] / matrix[k][k]) * matrix[k][size];
            matrix[i][k] = 0;
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

int main (int argc, char *argv[])
{
    int i, j;
    double start, end;
    
    if (argc != 2) {
        printf ("Please pass in the number of threads\n");
        return 1;
    }
    num_threads = atoi(argv[1]);
    Lab3LoadInput(&matrix, &size);
    printf("%d", size);
    
    for(i = 0; i < size; i++)
    {
        for(j = 0; j < size + 1; j++)
        {
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }

    printf("\n");

    GET_TIME(start);
    solve();
    GET_TIME(end);
    
    for(i = 0; i < size; i++)
    {
        for(j = 0; j < size + 1; j++)
        {
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }

    get_solution();

    for(j = 0; j < size; j++)
    {
        printf("%f ", vector[j]);
    }
    printf("\n");
    
    Lab3SaveOutput(vector, size, start-end);
    
    return 0;
}