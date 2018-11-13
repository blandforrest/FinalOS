#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMKEY ((key_t) 6164)
#define MAX 1000
#define PORT_NUM 2008

//Shared Memory
typedef struct{
    char shared_buffer[MAX];
} shared_memory;

void create_server();
void *messager(void *in_arg);

shared_memory *shared_mem_ptr;
pthread_mutex_t lock;