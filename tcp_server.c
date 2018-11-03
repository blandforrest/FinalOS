//**************//
//Curent Functionality
//
//Hosts communication between client and server
//One message at a time communication(bad)
//


#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>



#define MAX 1000



void create_server();
void messager(int new_socket);



int main(){
	create_server();
	return 0;
}



void create_server(){

    int server_fd, new_socket;
   	struct sockaddr_storage server_storage;
	socklen_t addr_size;
	struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
   
	
	// Creating socket file descriptor
	//Socket is using IPv4 and TCP. Checks if NULL
    if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

	//Address is a struct of sockaddr_in and uses IPv4 and the localhost for IP
    address.sin_family = AF_INET; 
    address.sin_port = htons(7891);
	address.sin_addr.s_addr = inet_addr("127.0.0.1");


    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
	printf("Listening...\n");
    if (listen(server_fd, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
	addr_size = sizeof server_storage;
	if ((new_socket = accept(server_fd, (struct sockaddr *) &server_storage, &addr_size)) < 0){
			perror("accept");
			exit(EXIT_FAILURE);
	}


	messager(new_socket);
}



void messager(int new_socket){
    char buffer[MAX] = {0}, message[MAX];
	char hello[] = 
"\n   *****************************\n\
   *                           *\n\
   *       Welcome to the      *\n\
   *           server!         *\n\
   *                           *\n\
   *                           *\n\
   *****************************\n\n\n\n\n";
 
	
	
	
	//Send server greeting message
	send(new_socket, hello, strlen(hello),0);


	//If user enters "exit" terminate
	while(1){
		
		recv(new_socket, buffer, 1024,0);
	
		if(strcmp(buffer, "exit") == 0){
			recv(new_socket, buffer, MAX, 0);
			printf("Client has left the server\n");
			return;
		}	
		
		printf("Other user: %s", buffer);

		//Input message from stdin
		printf(">>");
		fgets(message,MAX, stdin);
		printf("\n");
		
		if(strcmp(message,"exit") == 0){
			printf("Exiting...\n");
			send(new_socket, "exit\n", sizeof(message),0);
			return;
		}

		//Send and reset buffer
		send(new_socket, message, strlen(message), 0);
		memset(message, 0, sizeof message);
		memset(buffer, 0, 1024);
	}

	close(new_socket); 
    return;


}
