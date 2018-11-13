#include "tcp_server.h"




int main(){
	char *shmadd = (char *) 0;
	int shmid;
	//Creating shared memory segment
    if((shmid = shmget(SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0){
        perror("shmget");
        exit(1);
    }
    //Attaching total to shared memory
    if((shared_mem_ptr = (shared_memory *) shmat(shmid, shmadd, 0)) == (shared_memory *) -1){
        perror("shmat");
        exit(0);
    }
	
    create_server();
 
    shmctl(shmid, IPC_RMID, NULL);
    if(shmdt(shared_mem_ptr) == -1){
		perror("shmdt");
		exit(-1);
    }

	return 0;
}



void create_server(){
    
    int server_fd, new_socket;
   	struct sockaddr_storage server_storage;
    socklen_t addr_size;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_t thread_id;
    
    
    // Creating socket file descriptor
    //Socket is using IPv4 and TCP. Checks if NULL
    if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    //Address is a struct of sockaddr_in and uses IPv4 and the localhost for IP
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT_NUM);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // Forcefully attaching socket to the port PORT_NUM
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listening...\n");
    if (listen(server_fd, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    int remaining_connections = 3; // TODO: change while condition to check shared memory for notification to terminate?
    while (remaining_connections >= 0) {
        addr_size = sizeof(server_storage);
        if ((new_socket = accept(server_fd, (struct sockaddr *) &server_storage, &addr_size)) < 0){
            perror("accept");
            exit(EXIT_FAILURE);
            
        }
        if (pthread_create(&thread_id, NULL, messager, (void *)(intptr_t)new_socket) != 0) {
            perror("Unable to create thread to handle new connection");
            exit(EXIT_FAILURE);
            
        }
        remaining_connections--;
    }
}



void *messager(void *in_arg){
    
    int new_socket = (intptr_t) in_arg;
    char buffer[MAX] = {0};
    char *message = "testing, testing, 1, 2, 3..."; // Test response
    
	pthread_mutex_lock(&lock);
    recv(new_socket, buffer, 20,0);
    printf("%s", buffer);
	memset(buffer, 0, sizeof(buffer));
    pthread_mutex_unlock(&lock);


	char str[MAX];
	
	while(1){
	recv(new_socket, str, 15,0);
    printf("%s", str);
	memset(str, 0, sizeof(str));
	}
	/*int i = 0;
    while (buffer[i] != '\0')
    {
        str[i] = buffer[i];
	    buffer[i]; 
   		i+=1;
    }
    printf("%s", str);*/
    
    sleep(2);
    
    //Send message once reading is complete
    //send(new_socket, str, strlen(str), 0);   
 
    close(new_socket); 
    pthread_exit(NULL);
}
