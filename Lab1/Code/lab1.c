#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "lab1_IO.h"
#include "timer.h"

int** A;
int** B;
int** C;
int n;
int p;
int sqrtP;

/* For this function we pass in only the rank of the pthread. 
We calculate the bounds of the final matrix that this thread is responsible for using the number of threads P,
 then calculate and write those values.
*/
void *multiplyThread(void* voidRank) {
    int rank = (int)voidRank;

    int i, j, k;
    int size = n/sqrtP; 

    int x,y;
    x = rank/sqrtP;
    y = rank%sqrtP;

    for(i=(size*x); i<size*(x+1); i++){
        for(j=(size*y); j<size*(y+1); j++){
            for(k=0; k<n; k++){
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return NULL;
}

/* This is the main function which takes the number of threads used as parameter, loads the input matrices 
from data_input, and writes the result of the matrix multiplication to data_output. 
Please note that the command line argument p must be a perfect square and n % p == 0 where n is the size of the 
input matrices.
*/
int main(int argc, char* argv[]){
    // Start the timer
    double timeStart;
    GET_TIME(timeStart);

    p=atoi(argv[1]);

    int i, j;
    sqrtP = sqrt(p);
    
    // Load the input
    Lab1_loadinput(&A, &B, &n);

    // Initialize our output matrix C
    C = malloc(n * sizeof(int*));
    for (i = 0; i < n; i++)
        C[i] = malloc(n * sizeof(int));
    
    for (i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            C[i][j] = 0;
        } 
    }

    // Create our threads for performing the matrix multiplication
    pthread_t* thread;
    thread = malloc(p*sizeof(pthread_t));

    for(int i=0; i<p; i++){
        pthread_create(&thread[i], NULL, multiplyThread, (void*) i);
    }

    // Close our threads and free them
    for(int i=0; i<p; i++){
        pthread_join(thread[i], NULL);
    }

    free(thread);

    // Stop the timer and record the results
    double timeFinished;
    GET_TIME(timeFinished);
    Lab1_saveoutput(C, &n, (timeFinished-timeStart));

    return 0;
}