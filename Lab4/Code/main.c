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

int node_begin, node_end;
int block_size;

double damp_const;
struct node *nodehead;
double *last_results;
double *results;
double *local_results;
double *contribution;
double *merged_result;

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
    node_begin = mpi_rank * block_size;
    node_end = node_begin + block_size - 1;

    if (node_init(&nodehead, 0, nodecount)) return 254;
    return 0;
}

void init_values()
{
    int i;
    last_results = malloc(nodecount * sizeof(double));
    results = malloc(nodecount * sizeof(double));
    local_results = malloc(block_size * sizeof(double));
    contribution = malloc(nodecount * sizeof(double));
    merged_result = malloc(nodecount * sizeof(double));
    
    for(i = 0; i < nodecount; i++)
    {
        results[i] = 1.0 / nodecount;
    }

    update_contributions();
}

void update_contributions()
{
    int i;
    for ( i = 0; i < nodecount; ++i)
    {
        contribution[i] = results[i] / nodehead[i].num_out_links * DAMPING_FACTOR;
    }
}

void solve()
{
    int i, j;
    
    do{
        
        // printf("1\n");
        vec_cp(results, last_results, block_size);
        // update the value
        for ( i = 0; i < block_size; ++i){
            // printf("%d\n", i);

            local_results[i] = 0;
            for ( j = 0; j < nodehead[i].num_in_links; ++j){
                // printf("3111111\n");
                // printf("%d\n", nodehead[i].inlinks[j]);
                // printf("3414141\n");
                // printf("%d\n", contribution[nodehead[i].inlinks[j]]);

                local_results[i] += contribution[nodehead[mpi_rank*block_size+i].inlinks[j]];

            }
            // printf("forend\n");

            local_results[i] += damp_const;
        // printf("5\n");

        }
        // printf("4\n");
        // update and broadcast the contribution
        update_contributions();
        MPI_Allgather(&local_results, block_size, MPI_DOUBLE, merged_result, block_size, MPI_DOUBLE, MPI_COMM_WORLD);
    }while(rel_error(results, last_results, block_size) >= EPSILON);
}

int main (int argc, char *argv[])
{
    double start, end;
    int i, j;
    
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

    solve();

    // for(i = 0; i < block_size; i++)
    // {
    //     printf("Node %d: %f\n", i, results[i]);
    // }

    Lab4_saveoutput(results, nodecount, 0.0);
    MPI_Allgather(&results, block_size, MPI_DOUBLE, merged_result, block_size, MPI_DOUBLE, MPI_COMM_WORLD);
    MPI_Finalize();
    // printf("nodecount: %d\n", nodecount);

    // for(i = 0; i < nodecount; i++)
    // {
    //     printf("Node %d\n", i);
    //     printf("IncomingLinks: %d\n", nodehead[i].num_in_links);
    //     for(j = 0; j < nodehead[i].num_in_links; j++)
    //     {
    //         printf("%d ", nodehead[i].inlinks[j]);
    //     }
    //     printf("\n");
    //     printf("Num out links: %d\n", nodehead[i].num_out_links);
    // }
    
    return 0;
}