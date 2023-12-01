//COEN 146L : Lab3, step 4: concurrent TCP server that accepts and responds to multiple client connection requests, each requesting a file transfer
/*
* Name: Kayleigh Vu
* Date: 04/25/2023
* Title: Lab3 - Step 4
* Description: This program is the concurrent TCP server that accepts/receives multiple client server
requests by using threads.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

//Define the number of clients/threads that can be served
#define N 100
int threadCount = 0;
pthread_t clients[N]; //declaring N threads

//Declare socket and connection file descriptors.
int sock_fd, conn_fd;

//Declare receiving and sending buffers of size 10 bytes
char buf[10];

//Declare server address to which to bind for receiving messages and client address to fill in sending address
struct sockaddr_in serverAddr, clientAddr;

//Connection handler (thread function) for servicing client requests for file transfer
void* connectionHandler(void* sock){
   //declare buffer holding the name of the file from client
   char fbuf[20];
   
   //Connection established, server begins to read and write to the connecting client
   int conn_fd = *(int *) sock;
   printf("Connection Established with client IP: %s and Port: %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
   
	//receive name of the file from the client
   read(conn_fd, fbuf, sizeof(fbuf));
	
   //open file and send to client
     printf("Opening file %s\n", fbuf);
      FILE *src = fopen(fbuf, "r");
      printf("Opened file successfully\n");

   int byte_size;
   //read file and send to connection descriptor
   while ((byte_size = fread(&buf, 1, sizeof(buf), src)) > 0){
    write(conn_fd, buf, byte_size);
   }
   printf("File transfer complete\n");
   
   //close file
   fclose(src);
    
   //Close connection descriptor
   close(conn_fd);

   pthread_exit(0);
}


int main(int argc, char *argv[]){
 //Get from the command line, server IP, src and dst files.
 if (argc != 2){
	printf ("Usage: %s <port #> \n",argv[0]);
	exit(0);
 } 
 //Open a TCP socket, if successful, returns a descriptor
 sock_fd = socket(AF_INET, SOCK_STREAM, 0);
   if (sock_fd < 0){
      perror("Cannot create socket\n");
      return 0;
   }

 //Setup the server address to bind using socket addressing structure
bzero(&serverAddr, sizeof(serverAddr));
serverAddr.sin_family = AF_INET;
serverAddr.sin_port = htons(atoi(argv[1]));
serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

 //bind IP address and port for server endpoint socket
bind(sock_fd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

 // Server listening to the socket endpoint, and can queue 5 client requests
 listen(sock_fd, 5);
 printf("Server listening/waiting for client at port 50\n");
 
 while (1){
   //Server accepts the connection and call the connection handler
   socklen_t clientSize = sizeof(clientAddr);
   int new_socket = accept(sock_fd, (struct sockaddr *) &clientAddr, &clientSize);   //new socket accepts the client address
   
   if(pthread_create(&clients[threadCount], NULL, connectionHandler, (void*) &new_socket) < 0){
      perror("Unable to create a thread");
      exit(0);
   }
   else {
      printf("Thread %d has been created to service client request\n",++threadCount);
   }
 }
 int i;
 for(i = 0; i < threadCount; i++){
      pthread_join(clients[i], NULL);
 }
 return 0;
}