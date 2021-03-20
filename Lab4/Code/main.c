//main 
#define LAB4_EXTEND

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "timer.h"
#include "Lab4_IO.h"
#include <mpi.h>

#define DAMPING_FACTOR 0.85
#define EPSILON 0.00001

int nodecount;
int nodecount_padded;
int block_size;

double damp_const;
struct node *nodehead;
double *last_results;
double *results;
double *local_results;
double *contribution;

int mpi_size;
int mpi_rank;

int get_data()
{
    FILE *ip;

    if ((ip = fopen("data_input_meta", "r")) == NULL ){
    	printf("Error loading the data_output.\n");
        return 253;
    }
    fscanf(ip, "%d\n", &nodecount);
    fclose(ip);

    block_size = nodecount/mpi_size;
    if (nodecount%mpi_size) {
        block_size += 1;
    }
    nodecount_padded = block_size * mpi_size;

    if (node_init(&nodehead, 0, nodecount)) return 254;
    return 0;
}

void update_contributions()
{
    int i;
    for ( i = 0; i < nodecount; ++i)
    {
        contribution[i] = results[i] / nodehead[i].num_out_links * DAMPING_FACTOR;
    }
}

void init_values()
{
    int i;
    last_results = malloc(nodecount_padded * sizeof(double));
    results = malloc(nodecount_padded * sizeof(double));
    local_results = malloc(block_size * sizeof(double));
    contribution = malloc(nodecount * sizeof(double));
    
    for(i = 0; i < nodecount; i++)
    {
        results[i] = 1.0 / nodecount;
    }
    for(i = nodecount; i < nodecount_padded; i++) {
        results[i] = 0;
    }

    update_contributions();
}

void solve()
{
    int i, j;
    
    do{
        
        // printf("1\n");
        vec_cp(results, last_results, nodecount);
        // update the value
        for ( i = 0; i < block_size; ++i){
            // printf("%d\n", i);
            if (mpi_rank*block_size+i >= nodecount) {
                break;
            }

            local_results[i] = 0;
            for ( j = 0; j < nodehead[mpi_rank*block_size+i].num_in_links; ++j){

                local_results[i] += contribution[nodehead[mpi_rank*block_size+i].inlinks[j]];

            }

            local_results[i] += damp_const;
        }

        MPI_Allgather(local_results, block_size, MPI_DOUBLE, results, block_size, MPI_DOUBLE, MPI_COMM_WORLD);
        // update and broadcast the contribution
        update_contributions();
    }while(rel_error(results, last_results, nodecount) >= EPSILON);
}

int main (int argc, char *argv[])
{
    double start, end;
    
    if (argc != 1) {
        printf ("No Command line arguments required for this program\n");
        return 1;
    }

    MPI_Init(NULL, NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    get_data();
    damp_const = (1.0 - DAMPING_FACTOR) / nodecount;

    init_values();

    GET_TIME(start);
    solve();
    GET_TIME(end);

    if (mpi_rank == 0) {
        Lab4_saveoutput(results, nodecount, end-start);
    }
    
    MPI_Finalize();
    
    return 0;
}