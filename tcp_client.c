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

#define MAX  1000
#define PORT_NUM 2008

//Argument struct for pthread_create
struct arg_struct{
	const char *arg1;
	char   *arg2;
};

FILE *fp;

void delay();
void *run_client(void * arguments);
void create_threads(const char * ip_addr);
void delete_thread(pthread_t threadArray[3]);




int main(int argc, char **argv) {
    
    if(argc != 3) {
        printf("Usage: /{filename} {server_ip}\n");
        return -1;
    }
    char * ip_addr = argv[2];
    
	//Global FP
	fp = fopen("input.txt", "r");
	
	create_threads(ip_addr);
    

	return 0;
}

void create_threads(const char * ip_addr){
	char * thread_ID = "1";
	
	struct arg_struct arguments;
	arguments.arg1 = ip_addr;
	arguments.arg2 = thread_ID;

	//Initializing threads
    pthread_t threadArray[3];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    //Creating three writer threads
    int i;
    for(i = 0; i < 3; i++){
        if (pthread_create(&threadArray[i], NULL, run_client,(void *) &arguments) != 0){
            perror("Unable to create thread to handle new connection");
            exit(EXIT_FAILURE);
        }
    	arguments.arg2++;
	}
    delete_thread(threadArray);
}

void delete_thread(pthread_t threadArray[3]){
    int i;
    for(i = 0; i < 3; i++){
        pthread_join(threadArray[i],NULL);
    }
}




void delay(int number_of_seconds){
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;
    
    // Stroing start time
    clock_t start_time = clock();
    
    // looping till required time is not acheived
    while (clock() < start_time + milli_seconds);
}


//Takes IP address and ID of writer thread
void *run_client(void *arguments){
    char received[MAX], to_send[MAX];
    int net_socket, conn_stat, valread;
    struct sockaddr_in server_address;
    socklen_t  address_size;
    char word[15];
  	char init_mssg[20] = "Writer #";

	struct arg_struct *args = arguments;
 
	const char *ip_addr = args->arg1;
	char * thread_ID = args->arg2;

	strcat(init_mssg, thread_ID); 

    if((net_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Socket Failed");
        exit(EXIT_FAILURE);
    }
    
    //Server address is IPv4 and port is PORT_NUM
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT_NUM);
    server_address.sin_addr.s_addr = inet_addr(ip_addr);
    
    // address_size = sizeof server_address;
    if(connect(net_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1){
        printf("There was an error connecting to the socket\n");
        printf("ERRNO %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    // Receiving welcome message
    /*recv(net_socket, received, sizeof(received),0);
    printf("%s", received);
    memset(received, 0, sizeof(received));
    */
    
    if(fp == NULL)
    {
		printf("File Open Error\n");
        exit(EXIT_FAILURE);
    }
    fgets(word,15,fp);
    
   /* if(send(net_socket,word,strlen(word),0) < 0)
    {
        printf("Error: send() failed\n");
        exit(EXIT_FAILURE);
    }*/
    
    // Input message
    /*printf("%s: ", username);
    memset(to_send, 0, sizeof(to_send));
    fgets(to_send, MAX, stdin);
    printf("\n");*/
    
    //Sending message to server
    if(send(net_socket, init_mssg, strlen(init_mssg), 0) < 0) {
        printf("Error: send() failed\n");
        exit(EXIT_FAILURE);
    }
    
    // Receive reply and terminate
    if(recv(net_socket, received, sizeof(received), 0) < 0) {
        printf("Error: recv() failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Server: %s\n", received);
    
}
           
