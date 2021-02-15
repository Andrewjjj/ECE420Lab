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
    int writer;
    pthread_cond_t readers_proceed;
    pthread_cond_t writer_proceed;
    int pending_writers;
    pthread_mutex_t read_write_lock;
} rwlock_t;
char** string_array;
rwlock_t* rwlock_array;

void rwlock_init(rwlock_t* l)
{
    l -> readers = l -> writer = l -> pending_writers = 0;
    pthread_mutex_init(&(l->read_write_lock), NULL);
    pthread_cond_init(&(l->readers_proceed), NULL);
    pthread_cond_init(&(l->writer_proceed), NULL);
}

void rwlock_read_lock(rwlock_t* l)
{
    pthread_mutex_lock(&(l->read_write_lock));
    while(l->pending_writers>0 || l->writer>0)
    {
        pthread_cond_wait(&(l->readers_proceed), &(l->read_write_lock));
    }
    l->readers++;
    pthread_mutex_unlock(&(l->read_write_lock));
}

void rwlock_write_lock(rwlock_t* l)
{
    pthread_mutex_lock(&(l->read_write_lock));
    while(l->writer>0 || l->readers>0)
    {
        l->pending_writers++;
        pthread_cond_wait(&(l->writer_proceed), &(l->read_write_lock));
        l->pending_writers--;
    }
    l->writer++;
    pthread_mutex_unlock(&(l->read_write_lock));
}

void rwlock_unlock(rwlock_t* l)
{
    pthread_mutex_lock(&(l->read_write_lock));
    if(l->writer>0)
        l->writer = 0;
    else if(l->readers > 0)
        l->readers--;

    if (l -> readers > 0)
        pthread_cond_broadcast(&(l -> readers_proceed));
    else if ((l -> readers == 0) && (l -> pending_writers > 0))
        pthread_cond_signal(&(l -> writer_proceed));
    else if ((l -> readers == 0) && (l -> pending_writers==0))
        pthread_cond_broadcast(&(l -> readers_proceed)); pthread_mutex_unlock(&(l -> read_write_lock));
}

void *ServerExecute(void *args)
{
    int clientFileDescriptor=(int)args;
    char str[COM_BUFF_SIZE];

    read(clientFileDescriptor,str,COM_BUFF_SIZE);
    printf("reading from client:%s\n",str);

    ClientRequest cr;
    ParseMsg(str, &cr);

    char temp[COM_BUFF_SIZE];
    if(!cr.is_read)
    {
        rwlock_write_lock(&rwlock_array[cr.pos]);
        setContent(cr.msg, cr.pos, string_array);
        getContent(temp, cr.pos, string_array);
        write(clientFileDescriptor,temp,COM_BUFF_SIZE);
    } 
    else 
    {
        rwlock_read_lock(&rwlock_array[cr.pos]);
        getContent(temp, cr.pos, string_array);
        write(clientFileDescriptor,temp,COM_BUFF_SIZE);
    }
    rwlock_unlock(&rwlock_array[cr.pos]);
    close(clientFileDescriptor);
    pthread_exit(NULL);
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


    struct sockaddr_in sock_var;
    int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
    int clientFileDescriptor;
    int i;
    pthread_t t[1000];

    rwlock_array = malloc(arr_size*sizeof(rwlock_t));
    for(i=0; i<arr_size; i++)
        rwlock_init(&rwlock_array[i]);

    string_array = malloc(arr_size * COM_BUFF_SIZE);
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
        while(1)        //loop 
        {
            for(i=0;i<1000;i++)      //can support 1000 clients at a time
            {
                clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
                printf("Connected to client %d\n",clientFileDescriptor);
                pthread_create(&t[i],NULL,ServerExecute,(void *)(long)clientFileDescriptor);
                
            }
            for(i = 0; i < 1000; i++)
            {
                pthread_join(t[i], NULL);
            }
        }
        close(serverFileDescriptor);
    }
    else{
        printf("socket creation failed\n");
    }
    
    for (i = 0; i < arr_size; i ++){
        free(string_array[i]);
    }
    free(rwlock_array);
    free(string_array);


    return 0;
}