#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<time.h>
#include<string.h>
#include<arpa/inet.h>
#include <errno.h>

#define MAX  1000




void delay();
void run_client();



int main() {
	run_client();
	return 0;
}



void delay(int number_of_seconds){
	// Converting time into milli_seconds
	int milli_seconds = 1000 * number_of_seconds;

	// Stroing start time
	clock_t start_time = clock();

	// looping till required time is not acheived
	while (clock() < start_time + milli_seconds);
}



void run_client(){
	char recieved[MAX], to_send[MAX], username[MAX];
	int net_socket, conn_stat, valread;	
	struct sockaddr_in server_address;
	socklen_t  address_size;

	if((net_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		perror("Socket Failed");
		exit(EXIT_FAILURE);
	}

	//Server address is IPv4 and port is 9005 - Initialize server_address to NULL	
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(7891);	
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");	

	memset(server_address.sin_zero, '\0', sizeof server_address.sin_zero);

	address_size = sizeof server_address;
	if(connect(net_socket, (struct sockaddr *) &server_address, address_size) == -1){
		printf("There was an error connecting to the socket\n");
		printf("ERRNO %s",strerror(errno));
		exit(EXIT_FAILURE);

	}


	while (1) {
		recv(net_socket, recieved, sizeof(recieved),0);
		
		printf("Server: ");
		printf("%s\n", recieved);
		if (strcmp(recieved, "exit") == 0) {
			delay(3);
			return;
		}
		memset(recieved, 0, sizeof(recieved));

		//Sending message to server
		printf("%s: ", username);
		memset(to_send, 0, sizeof(to_send));
		
		fgets(to_send, MAX, stdin);
		printf("\n");
		
		//Sending message to server
		if (strcmp(to_send, "exit\n") == 0) {
			printf("Exiting...\n");
			send(net_socket, "exit", sizeof(username), 0);
			exit(0);
		}
		send(net_socket, to_send, strlen(to_send), 0);
	}

	return;


}
