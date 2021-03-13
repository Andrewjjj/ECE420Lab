//main 
#define LAB4_EXTEND

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "timer.h"
#include "Lab4_IO.h"


int nodecount;
struct node *nodehead;

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

int main (int argc, char *argv[])
{
    double start, end;
    int i, j;
    
    if (argc != 1) {
        printf ("No Command line arguments required for this program\n");
        return 1;
    }

    get_data();
    printf("nodecount: %d\n", nodecount);

    
    for(i = 0; i < nodecount; i++)
    {
        printf("Node %d\n", i);
        printf("IncomingLinks: %d\n", nodehead[i].num_in_links);
        for(j = 0; j < nodehead[i].num_in_links; j++)
        {
            printf("%d ", nodehead[i].inlinks[j]);
        }
        printf("\n");
        printf("Num out links: %d\n", nodehead[i].num_out_links);
    }
    
    return 0;
}