#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h> 
#include <string.h>
#include "timer.h"
#include "common.h"
//#define NUM_STR 1024
#define STR_LEN 20

pthread_mutex_t array_lock;
char** string_array;

void *ServerEcho(void *args)
{
    int clientFileDescriptor=(int)args;
    char str[20];

    read(clientFileDescriptor,str,20);
    printf("reading from client:%s\n",str);
    
    printf("does this work?\n");
    ClientRequest cr;
    ParseMsg(str, &cr);
    //printf("message parsed");
    
    if(!cr.is_read)
    {
        printf("getting mutex");
        pthread_mutex_lock(&array_lock);
        setContent(cr.msg, cr.pos, string_array);
        char* temp;
        getContent(temp, cr.pos, string_array);
        printf(temp);
        pthread_mutex_unlock(&array_lock);
    }
    write(clientFileDescriptor,cr.msg,20);
    close(clientFileDescriptor);

    return NULL;
}

int main (int argc, char* argv[]) 
{
    if (argc != 4) {
        printf("Please pass in the correct number of arguments\n");
        return 0;
    }
    pthread_mutex_init(&array_lock, NULL);

    int arr_size = atoi(argv[1]);
    char* server_ip = argv[2];
    int server_port = atoi(argv[3]);

    struct sockaddr_in sock_var;
    int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
    int clientFileDescriptor;
    int i;
    pthread_t t[1000];

    string_array = (char**) malloc(arr_size * sizeof(char*));
    for (i = 0; i < arr_size; i ++){
        string_array[i] = (char*) malloc(STR_LEN * sizeof(char));
        sprintf(string_array[i], "String %d: initial value", i);
        printf("%s\n\n", string_array[i]);
    }

    sock_var.sin_addr.s_addr=inet_addr(server_ip);
    sock_var.sin_port=server_port;
    sock_var.sin_family=AF_INET;
    if(bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
    {
        printf("socket has been created\n");
        listen(serverFileDescriptor,2000);
        while(1)        //loop infinity
        {
            for(i=0;i<1;i++)      //can support 1000 clients at a time
            {
                clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
                printf("Connected to client %d\n",clientFileDescriptor);
                pthread_create(&t[i],NULL,ServerEcho,(void *)(long)clientFileDescriptor);
            }
        }
        close(serverFileDescriptor);
    }
    else{
        printf("socket creation failed\n");
    }

    return 0;
}