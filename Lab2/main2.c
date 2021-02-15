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

pthread_mutex_t* mutex_array;
char** string_array;

double time_arr[COM_NUM_REQUEST];
int time_index;
pthread_mutex_t time_lock;

void *ServerEchoMultiMutex(void *args)
{
    int clientFileDescriptor=(int)args;
    char str[COM_BUFF_SIZE];

    double start_time;
    double end_time;

    read(clientFileDescriptor,str,COM_BUFF_SIZE);
    printf("reading from client:%s\n",str);
    
    ClientRequest cr;
    ParseMsg(str, &cr);
    GET_TIME(start_time);
    pthread_mutex_lock(&mutex_array[cr.pos]);

    char temp[COM_BUFF_SIZE];
    if(!cr.is_read)
    {
        setContent(cr.msg, cr.pos, string_array);
        getContent(temp, cr.pos, string_array);
        GET_TIME(end_time);
        write(clientFileDescriptor,temp,COM_BUFF_SIZE);
    } else 
    {
        getContent(temp, cr.pos, string_array);
        GET_TIME(end_time);
        write(clientFileDescriptor,temp,COM_BUFF_SIZE);
    }
    pthread_mutex_unlock(&mutex_array[cr.pos]);
    
    close(clientFileDescriptor);

    pthread_mutex_lock(&time_lock);
    time_arr[time_index] = end_time - start_time;
    time_index += 1;
    time_index = time_index % COM_NUM_REQUEST;
    pthread_mutex_unlock(&time_lock);

    return NULL;
}

int main (int argc, char* argv[]) 
{
    if (argc != 4) {
        printf("Please pass in the correct number of arguments\n");
        return 0;
    }

    int arr_size = atoi(argv[1]);
    char* server_ip = argv[2];
    int server_port = atoi(argv[3]);
    int i;

    mutex_array = (pthread_mutex_t*) malloc(arr_size * sizeof(pthread_mutex_t));
    for (i = 0; i < arr_size; i++) {
        pthread_mutex_init(&mutex_array[i], NULL);
    }
    pthread_mutex_init(&time_lock, NULL);

    struct sockaddr_in sock_var;
    int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
    int clientFileDescriptor;
    pthread_t t[COM_NUM_REQUEST];

    string_array = (char**) malloc(arr_size * sizeof(char*));
    for (i = 0; i < arr_size; i ++){
        string_array[i] = (char*) malloc(COM_BUFF_SIZE * sizeof(char));
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
            for(i=0;i<COM_NUM_REQUEST;i++)      //can support 1000 clients at a time
            {
                clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
                printf("Connected to client %d\n",clientFileDescriptor);
                pthread_create(&t[i],NULL,ServerEchoMultiMutex,(void *)(long)clientFileDescriptor);
            }
            for(i=0;i<COM_NUM_REQUEST;i++)
            {
                pthread_join(t[i], NULL);
            }
            saveTimes(time_arr,COM_NUM_REQUEST);
        }
        close(serverFileDescriptor);
    }
    else{
        printf("socket creation failed\n");
    }


    for (i = 0; i < arr_size; i ++){
        free(string_array[i]);
    }
    free(string_array);
    free(mutex_array);

    return 0;
}