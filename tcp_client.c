#include "tcp_client.h"

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
	
	struct arg_struct arguments[3];
		arguments[0].arg2 = 1;
		arguments[1].arg2 = 2;
		arguments[2].arg2 = 3;

	//Initializing threads
    pthread_t threadArray[3];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    //Creating three writer threads
    int i;
    for(i = 0; i < 3; i++){
		
	arguments[i].arg1 = ip_addr;
		
        if (pthread_create(&threadArray[i], NULL, run_client,(void *) &arguments[i]) != 0){
            perror("Unable to create thread to handle new connection");
            exit(EXIT_FAILURE);
        }
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

	
	struct arg_struct *args = arguments;
	const char *ip_addr = args->arg1;

	printf("%d\n", args->arg2); 

	//Creating the writer message - "Writer thread#" - To be sent to server
  	char init_mssg[100] = "Writer #";	
	char thread_ID[20];
	sprintf(thread_ID, "%d", args->arg2);
	strcat(init_mssg, thread_ID);
	strcat(init_mssg, "\n");
	
	printf("%s\n", init_mssg);
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
    
    
    if(fp == NULL){
		printf("File Open Error\n");
        exit(EXIT_FAILURE);
    }
	
    
	//Using lock to let only one message go to server at a time 
    pthread_mutex_lock(&message_lock);
    if(send(net_socket, init_mssg, strlen(init_mssg), 0) < 0) {
        printf("Error: send() failed\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_unlock(&message_lock);
	
	
	//Reading from file and sending to server
    while(fgets(word,15,fp) != NULL){
		if(send(net_socket, word, strlen(init_mssg), 0) < 0) {
			printf("Error: send() failed\n");
			exit(EXIT_FAILURE);
		}
	}
	
	
    // Receive reply and terminate
    if(recv(net_socket, received, sizeof(received), 0) < 0) {
        printf("Error: recv() failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Server: %s\n", received);
    
}
           
