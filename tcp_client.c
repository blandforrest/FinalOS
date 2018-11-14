#include "tcp_client.h"

int main(int argc, char **argv) {
    
    if(argc != 2) {
        printf("Usage: /{filename} {server_ip}\n");
        return -1;
    }
    char * ip_addr = argv[1];
    
	//Global FP
	fp = fopen("input.txt", "r");

	create_threads(ip_addr);
        
    fclose(fp);

	return 0;
}

void create_threads(const char * ip_addr){
	
	//Define ID for each thread
	struct arg_struct arguments[3];
		arguments[0].arg2 = 1;
		arguments[1].arg2 = 2;
		arguments[2].arg2 = 3;

	//Initializing threads
    pthread_t threadArray[3];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    //Creating three client threads
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

void delete_thread(pthread_t threadArray[3]) {
    int i;
    for(i = 0; i < 3; i++){
        pthread_join(threadArray[i],NULL);
    }
}



//Takes IP address and ID of writer thread
void *run_client(void *arguments){
    int net_socket;
    struct sockaddr_in server_address;
	struct arg_struct *args = arguments;
	const char *ip_addr = args->arg1;
    char received[MAX], word[15];
    socklen_t  address_size;
 

    //Creating the writer message - "Writer thread#" - To be sent to server
  	char init_mssg[100] = "Connected to Writer #";	
	char thread_ID[20];
	sprintf(thread_ID, "%d", args->arg2);
	strcat(init_mssg, thread_ID);
	strcat(init_mssg, "\n");

	
    if((net_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
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

	//Checking for file error
    if(fp == NULL) {
		printf("File Open Error\n");
        exit(EXIT_FAILURE);
    }
	
	//Sends initial message to server
    if(send(net_socket, init_mssg, strlen(init_mssg), 0) < 0) {
        printf("Error: send() failed\n");
        exit(EXIT_FAILURE);
     }
	
	
	//Reading from file and sending to server
    fgets(word, 13, fp);
	strcat(word, "\\");
	strcat(word, thread_ID);
	strcat(word, "\0");

    // Send message
    if(send(net_socket, word, sizeof(word), 0) < 0) {
        perror("send");
        exit(EXIT_FAILURE);
    }
	
    // Receive reply and terminate
    if(recv(net_socket, received, sizeof(received), 0) < 0) {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", received);

    if (close(net_socket) < 0) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    pthread_exit(NULL);
    
}
           
