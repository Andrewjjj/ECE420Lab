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

#define True 1
#define False 0
typedef struct {
    int readers;
    int writer_present;
    pthread_cond_t readers_proceed;
    pthread_cond_t writers_proceed;
    int pending_writers;
    pthread_mutex_t read_write_lock;
} rw_lock;
char** string_array;
rw_lock mylock;

pthread_mutex_t index_mutex;
int ind;
double timing[1000];

void rw_lock_init(rw_lock* l)
{
    l->readers = 0;
    pthread_mutex_init(&(l->read_write_lock), NULL);
    pthread_cond_init(&(l->readers_proceed), NULL);
    pthread_cond_init(&(l->writers_proceed), NULL);
}

void read_lock(rw_lock* l)
{
    pthread_mutex_lock(&(l->read_write_lock));
    while(l->pending_writers > 0 || l->writer_present)
    {
        pthread_cond_wait(&(l->readers_proceed), &(l->read_write_lock));
    }
    l->readers += 1;
    pthread_mutex_unlock(&(l->read_write_lock));
}

void write_lock(rw_lock* l)
{
    pthread_mutex_lock(&(l->read_write_lock));
    while(l->writer_present || l->readers > 0)
    {
        l->pending_writers += 1;
        pthread_cond_wait(&(l->writers_proceed), &(l->read_write_lock));
        l->pending_writers -= 1;
    }
    l->writer_present = True;
    pthread_mutex_unlock(&(l->read_write_lock));
}

void rw_unlock(rw_lock* l)
{
    pthread_mutex_lock(&(l->read_write_lock));
    if(l->writer_present)
    {
        l->writer_present = False;
    }
    else if(l->readers > 0)
    {
        l->readers -= 1;
    }

    if(l->readers > 0)
    {
        pthread_cond_broadcast(&(l->readers_proceed));
    }
    else if(l->readers == 0 && l->pending_writers > 0)
    {
        pthread_cond_broadcast(&(l->writers_proceed));
    }
    else if(l->readers == 0 && l->pending_writers == 0)
    {
        pthread_cond_broadcast(&(l->readers_proceed));
    }
    pthread_mutex_unlock(&(l->read_write_lock));
}

void *ServerExecute(void *args)
{
    double start_time;
    double end_time;

    int clientFileDescriptor=(int)args;
    char str[COM_BUFF_SIZE];

    read(clientFileDescriptor,str,COM_BUFF_SIZE);
    //printf("reading from client:%s\n",str);
    GET_TIME(start_time);

    ClientRequest cr;
    ParseMsg(str, &cr);

    char temp[COM_BUFF_SIZE];
    if(!cr.is_read)
    {
        write_lock(&mylock);
        setContent(cr.msg, cr.pos, string_array);
        getContent(temp, cr.pos, string_array);
        GET_TIME(end_time);
        write(clientFileDescriptor,temp,COM_BUFF_SIZE);
    } 
    else 
    {
        read_lock(&mylock);
        getContent(temp, cr.pos, string_array);
        GET_TIME(end_time);
        write(clientFileDescriptor,temp,COM_BUFF_SIZE);
    }

    rw_unlock(&mylock);
    close(clientFileDescriptor);
    pthread_mutex_lock(&index_mutex);
    timing[ind] = end_time - start_time;
    ind += 1;
    ind = ind % COM_NUM_REQUEST;
    pthread_mutex_unlock(&index_mutex);
    pthread_exit(NULL);
    return NULL;
}

int main (int argc, char* argv[])
{
    if (argc != 4) {
        printf("Please pass in the correct number of arguments\n");
        return 0;
    }
    rw_lock_init(&mylock);
    pthread_mutex_init(&index_mutex, NULL);
    ind = 0;

    int arr_size = atoi(argv[1]);
    char* server_ip = argv[2];
    int server_port = atoi(argv[3]);

    struct sockaddr_in sock_var;
    int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
    int clientFileDescriptor;
    int i;
    pthread_t t[1000];

    string_array = malloc(arr_size * COM_BUFF_SIZE);
    for (i = 0; i < arr_size; i ++){
        string_array[i] = (char*) malloc(COM_BUFF_SIZE * sizeof(char));
        sprintf(string_array[i], "String %d: initial value", i);
        //printf("%s\n\n", string_array[i]);
    }

    sock_var.sin_addr.s_addr=inet_addr(server_ip);
    sock_var.sin_port=server_port;
    sock_var.sin_family=AF_INET;
    if(bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
    {
        printf("socket has been created\n");
        listen(serverFileDescriptor,2000);
        while(1)        //loop 
        {
            for(i=0;i<COM_NUM_REQUEST;i++)      //can support 1000 clients at a time
            {
                clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
                //printf("Connected to client %d\n",clientFileDescriptor);
                pthread_create(&t[i],NULL,ServerExecute,(void *)(long)clientFileDescriptor);
            }
            for(i = 0; i < COM_NUM_REQUEST; i++)
            {
                pthread_join(t[i], NULL);
            }
            saveTimes(timing, 1000);
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

    return 0;
}