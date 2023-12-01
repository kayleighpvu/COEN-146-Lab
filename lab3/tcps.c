//COEN 146L : Lab3, step 1: TCP server that accepts a client connection for file transfer.
/*
* Name: Kayleigh Vu
* Date: 04/25/2023
* Title: Lab3 - Step 1
* Description: This program is the server side of the network which waits for contact from 
the client, connects, sends ACK to client that connection has been established, 
and then writes the data from the source file to an empty destination file.  Then sends another ack
to the client that file transfer has been complete. TCP/IP connection sends ACKs.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>

//Declare socket file descriptor.
int sockfd;

//Declare receiving and sending buffers of size 10 bytes
char buf[10];

//Declare server address to which to bind for receiving messages and client address to fill in sending address
struct sockaddr_in server_addr, client_addr;

//Connection handler for servicing client request for file transfer
void* connectionHandler(void* sock){
   //declate buffer holding the name of the file from client
   char file_buf[20];

   //get the connection descriptor
   int connfd = *(int *) sock;

   //Connection established, server begins to read and write to the connecting client
   printf("Connection Established with client IP: %s and Port: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
   
	//receive name of the file from the client
   read(connfd, file_buf, sizeof(file_buf));

   //open file and send to client
   printf("Opening file %s\n", file_buf);
   FILE *fd = fopen(file_buf, "r");
   printf("Opened file successfully\n");

   int byteSize;
   //read file and send to connection descriptor
   while ((byteSize = fread(&buf, 1, sizeof(buf), fd)) > 0){
    write(connfd, buf, byteSize);
   }

   printf("File transfer complete\n");
   
   //close file
   fclose(fd);

   //Close connection descriptor
   close(connfd);
   
   return 0;
}


int main(int argc, char *argv[]){
   //Get from the command line, server IP, src and dst files.
   if (argc != 2){
      printf ("Usage: %s <port #> \n",argv[0]);
      exit(0);
   } 
   //Open a TCP socket, if successful, returns a descriptor
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0){
      perror("Cannot create socket\n");
      return 0;
   }

   //Setup the server address to bind using socket addressing structure
   bzero(&server_addr, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

   //bind IP address and port for server endpoint socket 
   bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));

   // Server listening to the socket endpoint, and can queue 5 client requests
   listen(sockfd,5);
   printf("Server listening/waiting for client at port %s\n", argv[1]);

   //Server accepts the connection and call the connection handler
   socklen_t clientSize = sizeof(client_addr);
   int new_socket = accept(sockfd, (struct sockaddr *) &client_addr, &clientSize);   //new socket accepts the client address
   connectionHandler(&new_socket);

   //close socket descriptor
   close(sockfd);

   return 0;
}


//./server port#

//./client 127.0.0.1 port# src.dat dst.dat