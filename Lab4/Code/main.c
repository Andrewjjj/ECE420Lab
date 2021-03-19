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
double damp_const;
struct node *nodehead;
double *last_results;
double *results;
double *contribution;

int get_data()
{
    FILE *ip;

    if ((ip = fopen("data_input_meta", "r")) == NULL ){
    	printf("Error loading the data_output.\n");
        return 253;
    }
    fscanf(ip, "%d\n", &nodecount);
    fclose(ip);

    if (node_init(&nodehead, 0, nodecount)) return 254;
    return 0;
}

void init_values()
{
    int i;
    last_results = malloc(nodecount * sizeof(double));
    results = malloc(nodecount * sizeof(double));
    contribution = malloc(nodecount * sizeof(double));

    for(i = 0; i < nodecount; i++)
    {
        results[i] = 1.0 / nodecount;
    }
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
        vec_cp(results, last_results, nodecount);
        // update the value
        for ( i = 0; i < nodecount; ++i){
            results[i] = 0;
            for ( j = 0; j < nodehead[i].num_in_links; ++j)
                results[i] += contribution[nodehead[i].inlinks[j]];
            results[i] += damp_const;
        }
        // update and broadcast the contribution
        update_contributions();
    }while(rel_error(results, last_results, nodecount) >= EPSILON);
}

int main (int argc, char *argv[])
{
    double start, end;
    int i, j;
    
    if (argc != 1) {
        printf ("No Command line arguments required for this program\n");
        return 1;
    }

    get_data();
    damp_const = (1.0 - DAMPING_FACTOR) / nodecount;

    init_values();

    solve();

    for(i = 0; i < nodecount; i++)
    {
        printf("Node %d: %f\n", i, results[i]);
    }

    Lab4_saveoutput(results, nodecount, 0.0);

    //printf("nodecount: %d\n", nodecount);

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