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
int convP;

void *multiplyThread(void* voidRank) {

    int rank = (int)voidRank;

    int i, j, k;
    int size = n/convP; 

    int x,y;
    x = rank/convP;
    y = rank%convP;

    for(i=(size*x); i<size*(x+1); i++){
        for(j=(size*y); j<size*(y+1); j++){
            for(k=0; k<n; k++){
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return NULL;
}


int main(int argc, char* argv[]){
    double timeStart;
    GET_TIME(timeStart);

    p=atoi(argv[1]);

    int i, j, k;
    FILE* fp;
    int temp,flag=1;
    convP = sqrt(p);
    
    Lab1_loadinput(&A, &B, &n);

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

    pthread_t* thread;
    thread = malloc(p*sizeof(pthread_t));

    for(int i=0; i<p; i++){
        pthread_create(&thread[i], NULL, multiplyThread, (void*) i);
    }

    for(int i=0; i<p; i++){
        pthread_join(thread[i], NULL);
    }
    free(thread);
    double timeFinished;
    GET_TIME(timeFinished);
    Lab1_saveoutput(C, &n, (timeFinished-timeStart));

    return 0;
}