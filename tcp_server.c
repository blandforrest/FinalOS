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
 
    // detach shared memory
    if(shmdt(shared_mem_ptr) == -1){
		perror("shmdt");
		exit(-1);
    }

    // remove shared memory
    shmctl(shmid, IPC_RMID, NULL);

	return 0;
}



void create_server(){
    
    int server_fd, new_socket;
   	struct sockaddr_storage server_storage;
    socklen_t addr_size;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t thread_id;
    
    // Socket is using IPv4 and TCP
    if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Address is a struct of sockaddr_in and uses IPv4 and the localhost for IP
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT_NUM);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // Forcefully attaching socket to the port PORT_NUM
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT_NUM);
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
	
	//Initializing threads
    pthread_t threadArray[3];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
	
	int i;
	for(i = 0; i < 3; i++){		
        addr_size = sizeof(server_storage);
        if ((new_socket = accept(server_fd, (struct sockaddr *) &server_storage, &addr_size)) < 0){
            perror("accept");
            exit(EXIT_FAILURE);
            
        }
        if (pthread_create(&threadArray[i], NULL, messager, (void *)(intptr_t)new_socket) != 0) {
            perror("Unable to create thread to handle new connection");
            exit(EXIT_FAILURE);
            
        }
	}
    // Wait until threads are done - then join and close server
	delete_thread(threadArray);

    if (close(server_fd) < 0)
    {
        perror("close");
        exit(EXIT_FAILURE);
    }
}



void delete_thread(pthread_t threadArray[3]){
    int i;
    for(i = 0; i < 3; i++){
        pthread_join(threadArray[i],NULL);
    }
}



void *messager(void *in_arg){
    
    int new_socket = (intptr_t) in_arg;
    char buffer[MAX], init_message[100];
    
	// Recieving and printing init_message from writers
	if (recv(new_socket, init_message, sizeof(init_message),0) <= 0){ 
		perror("recv");
        exit(EXIT_FAILURE);
    }
	printf("%s", init_message);
	
	//Recieving file data from server 
    if (recv(new_socket, buffer, MAX,0) <= 0){ 
        perror("recv");
        exit(EXIT_FAILURE);
    }
	pthread_mutex_lock(&lock);
	
	//Copy buffer to shared memory and printf
    strcpy(shared_mem_ptr->shared_buffer, buffer);
    printf("%s\n", shared_mem_ptr->shared_buffer);
    
    sleep(2);
    
    //Send message once reading is complete
    send(new_socket, shared_mem_ptr->shared_buffer, sizeof(shared_mem_ptr->shared_buffer), 0); 
	
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}
