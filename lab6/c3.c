//COEN 146L: Lab6, UDP client with packets that have checksum, sequence number, acknowledgement number, and timer
/*
* Name: Kayleigh Vu
* Date: 05/16/2023
* Title: Lab6 - UDP Client
* Description: Similar function to lab 5 but the client additionally includes a timer
* to know if it should retransmit the packet or has the received the ACK from the server
* to send the next packet.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

//Declare a Header structure that holds, sequence/ acknowledgement number, checksum, and length of a packet
typedef struct {
    int seq_ack;
    int len;
    int cksum;
} Header;

//Declare a packet structure that holds data and header
typedef struct {
    Header header;
    char data[10];
} Packet;

//Calculate the Checksum
int getChecksum(Packet packet) {
    packet.header.cksum = 0;
    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
    while (ptr < end) {
        checksum ^= *ptr++;
    } 
        return checksum;
}

//Print received packet
void printPacket(Packet packet) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
            packet.header.seq_ack,
            packet.header.len,
            packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}

//client sending packet with checksum and sequence number, waits for acknowledgement, and sets up a time
void clientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet, unsigned retries) {
    while (1) {	
		//if retries is greater than 3, we give up and move on
        if(retries >= 3){
            break;
        }
        //calculate checksum of packet
        int e_cksum;
        e_cksum = getChecksum(packet);

        //Simulate loss of a packet
        if(rand() % 5 == 0) //simulate a drop of packet (probability = 20%)
            printf("Dropping packet\n");
        else{
            //send the packet
            printf("Client sending packet\n");
            sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);
        }
        
        // wait until an ACK is received or timeout using select statement      
        // local variables needed
        struct  timeval tv; // timer
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        int rv;     // select returned value

        // set up reads file descriptor at the beginning of the function to be checked for being ready to read
        fd_set  readfds;
        fcntl (sockfd, F_SETFL, O_NONBLOCK);

        // start before calling select
        FD_ZERO (&readfds); //initializes readfds to have zero bits
        FD_SET (sockfd, &readfds); //sets readfds bit 

        //call select
        rv = select(sockfd + 1, &readfds, NULL, NULL, &tv);
        
        if(rv == 0){
            printf("Timeout\n");
            //increment retries if packet is dropped
            retries++;
        }
        //so, there is an ACK to receive
        else {
            //receive an ACK from the server
            Packet recvpacket;
            recvfrom(sockfd, &recvpacket, sizeof(recvpacket), 0, (struct sockaddr *) address, &addrlen);
                        
            //print received packet (ACK) and checksum
            printf("Client received ACK %d, checksum %d - \n", recvpacket.header.seq_ack, recvpacket.header.cksum);

            //calculate checksum of received packet (ACK)
            int e_cksum;
            e_cksum = getChecksum(recvpacket);

            //check the checksum
            if (recvpacket.header.cksum != e_cksum) {
            //if bad checksum, resend packet
                printf("Client: Bad checksum, expected checksum was: %d\n", e_cksum);
                sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);
            }

            //check the sequence number
            else if (packet.header.seq_ack != recvpacket.header.seq_ack){
                //if incorrect sequence number, resend packet
                printf("Client: Bad seqnum, expected sequence number was: %d\n",packet.header.seq_ack);
                sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);
            }

            //if the ckechsum and sequence numbers are correct, break and return to the main to get the next packet to send
            else {
                //good ACK, we're done
                printf("Client: Good ACK\n");
                break;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    //Get from the command line, server IP, Port and src file
    if (argc != 4) {
        printf("Usage: %s <ip> <port> <srcfile>\n", argv[0]);
        exit(0);
    }
    
    //Declare socket file descriptor.
    int sockfd; 

    //Open a UDP socket, if successful, returns a descriptor    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
      printf("Cannot create socket\n");
      return 0;
    }

    //Declare server address to connect to
    struct sockaddr_in servAddr;
    
    //initialize servAddr structure
    servAddr.sin_port = htons(atoi(argv[2]));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //Open file using argv[3]
    int fp = open(argv[3], O_RDWR);
    if(fp < 0){
    	perror("Failed to open file\n");
	    exit(1);
    }

    //Send file contents to server packet by packet
    int seq = 0;
    socklen_t addr_len = sizeof(servAddr);
    Packet packet;
    int bytes;
    while((bytes = read(fp, packet.data, sizeof(packet.data))) > 0){
    	//assign seq and checksum to packet and send
        packet.header.seq_ack=seq;
    	packet.header.len=bytes;
    	packet.header.cksum=getChecksum(packet);
    	clientSend(sockfd,(struct sockaddr *)&servAddr,addr_len, packet, 0); //retries = 0
    	seq=(seq+1)%2;
    }

    //Send zero-length packet to server to end connection
    Packet final;
    final.header.seq_ack=seq;
    final.header.len=0;
    final.header.cksum=getChecksum(final);
    clientSend(sockfd,(struct sockaddr *)&servAddr,addr_len,final, 0);
    
	//Close file and socket
    close(fp);
    close(sockfd);
    return 0;
}
