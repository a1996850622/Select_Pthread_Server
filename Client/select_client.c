#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>	// For sockaddr_in
#include <string.h>	// For string handle function
#include <arpa/inet.h>	// For address conversion function
#include <unistd.h>
#include <fcntl.h>

// Used to print error messages
#define PANIC(msg) { perror(msg); exit(-1); }
#define STDIN 0

#define BUFSIZE 256 // Define the buffer size

int main(int argc, char *argv[]){
	int sockfd;
	int retval; // The return value of select()
	int i; // Just for "for" loop
	int fdmax;
	int fd;
	int recvnum;
	int flag=0;
	char trans_buf[BUFSIZE]; // Put the data that you will send
	char recv_buf[BUFSIZE]; // Put the data that you received
	char filename[BUFSIZE];
	char file_buf[1024];
	struct sockaddr_in dest;
	struct timeval tv; // Set the timeout time
	fd_set read_fds; // fd set

	// Generate an socketfd of IPv4 + TCP connection
	sockfd=socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd==-1) PANIC("socket");

	// Empty the struct of server_addr
	memset(&dest, 0, sizeof(dest));
	dest.sin_family=AF_INET; // IPv4
	dest.sin_port=htons(9999); // Set port number
	dest.sin_addr.s_addr=inet_addr("127.0.0.1"); // Set IP address

	puts("Login...");
	puts("If you want to download files from server, you can key [require+filename]");

	// Build the connection with Server, and we will be accepted
	if(connect(sockfd, (struct sockaddr*)&dest, sizeof(dest))==-1)
		PANIC("connect")

	fdmax=sockfd;

	for(;;){

		FD_ZERO(&read_fds);	// Empty the read_fds

		FD_SET(STDIN,&read_fds); // Put STDIN into read_fds
		FD_SET(sockfd,&read_fds); // Put sockfd into read_fds

		retval=select(fdmax+1, &read_fds, NULL, NULL, NULL);
		switch(retval){ 
			case -1: // When retval=-1, it means that some errors occur
				perror("select");
				continue;

			case 0: // When retval=0, it means overtime
				printf("Time Out...\n");

				/* 
				 * Close all fd in master_fds, 
				 * but except "stdin" "stdout" "stderr".
				 * Then exit the process
				 */
				for(i=3; i<= fdmax; i++)
					if (FD_ISSET(i,&read_fds)) 
						close(i);
				exit(0); 
		}
	
		// If you key a string in your terminal, it will enter "if"
		if(FD_ISSET(STDIN, &read_fds)){
			memset(trans_buf, 0, BUFSIZE); // Empty the buffer

			fgets(trans_buf, BUFSIZE, stdin); // Get a string from your terminal

			if(strstr(trans_buf, "require")!=NULL){
				// Parse filename
				sscanf(trans_buf, "require %s\n", filename);
				printf("You require the [%s] file from Server\n", filename);
				flag=1;
			}

			// Send the data of trans_buf to the server
			send(sockfd, trans_buf, BUFSIZE, 0);
		}

		// If the server sends me a message, it will enter "if"
		if(FD_ISSET(sockfd, &read_fds)){
			memset(recv_buf, 0, BUFSIZE); // Empty the buffer

			// Receive the message from server
			recvnum=recv(sockfd, recv_buf, sizeof(recv_buf), 0);
			if(recvnum==-1) perror("recv");

			printf("%s", recv_buf);

			if(flag==1){
				// Create the file which I required.
				fd=open(filename, O_WRONLY|O_CREAT, 0666);
				switch(fd){
					case -1:
						perror("open");
						flag=0;
						continue;
					default:
						// Save the data to the file
						write(fd, recv_buf, recvnum);
						printf("[%s] save successfully.\n", filename);
						flag=0;
						continue;
				}
			}
		}
	}
	close(sockfd);
	return 0;
}
