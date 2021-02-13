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
#define NUM_STR 1024
#define STR_LEN 1000

void *ServerEcho(void *args)
{
    int clientFileDescriptor=(int)args;
    char str[20];

    read(clientFileDescriptor,str,20);
    printf("reading from client:%s\n",str);
    write(clientFileDescriptor,str,20);
    close(clientFileDescriptor);
    printf("does this work?\n");
    ClientRequest cr;
    ParseMsg(str, &cr);
    printf("%s\n", cr.msg);

    return NULL;
}

int main (int argc, char* argv[]) 
{
    if (argc != 4) {
        printf("Please pass in the correct number of arguments\n");
        return 0;
    }

    int arr_size = atoi(argv[1]);
    int server_ip = argv[2];
    int server_port = argv[3];

    struct sockaddr_in sock_var;
    int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
    int clientFileDescriptor;
    int i;
    pthread_t t[20];

    sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
    sock_var.sin_port=3000;
    sock_var.sin_family=AF_INET;
    if(bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
    {
        printf("socket has been created\n");
        listen(serverFileDescriptor,2000); 
        while(1)        //loop infinity
        {
            for(i=0;i<20;i++)      //can support 20 clients at a time
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