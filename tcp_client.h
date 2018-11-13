#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<time.h>
#include<string.h>
#include<arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#define MAX 15
#define PORT_NUM 2008

//Argument struct for pthread_create
struct arg_struct{
	const char *arg1;
	int   arg2;
};

FILE *fp;
pthread_mutex_t message_lock;

void delay();
void *run_client(void * arguments);
void create_threads(const char * ip_addr);
void delete_thread(pthread_t threadArray[3]);