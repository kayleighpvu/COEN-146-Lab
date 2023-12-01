//COEN 146L : Lab3, step 1: TCP client requesting a file transfer from the server.
/*
* Name: Kayleigh Vu
* Date: 04/25/2023
* Title: Lab3 - Step 2
* Description: This program is the client side of the network which will initiate contact 
with the server, the client's inputs of the IP address, port number, source file name, and destination
file name will be given to the server for the server to retrieve it to copy.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){
  //Get from the command line, server IP, src and dst files.
  if (argc != 5){
	  printf ("Usage: %s <ip of server> <port #> <src_file> <dest_file>\n",argv[0]);
	  exit(0);
  } 
  //Declare socket file descriptor and buffer
  int sockfd;
  char buf[10];

  //Declare server address to accept
  struct sockaddr_in server_addr;

  //Declare host
  struct hostent *host;

  //get hostname
  host = (struct hostent *) gethostbyname(argv[1]);

  //Open a socket, if successful, returns
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0){
      printf("Cannot create socket\n");
      return 0;
   }

  //Set the server address to send using socket addressing structure
  server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));  //getting argument 2 (port number)
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);

  socklen_t serverSize = sizeof(server_addr);
  //Connect to the server
  connect(sockfd, (struct sockaddr *) &server_addr, serverSize);
  
  //Client sends the name of the file to retrieve from the server
  write(sockfd,argv[3],strlen(argv[3])+1);

  //Copying from source file to destination file
  FILE *dst_file = fopen(argv[4], "w");
  int biteSize;
  while((biteSize = read(sockfd, buf, sizeof(buf))) != 0){
    fwrite(buf, 1, biteSize, dst_file);
  }

  //Close socket descriptor
  close(sockfd);
  fclose(dst_file);

  return 0;
}

