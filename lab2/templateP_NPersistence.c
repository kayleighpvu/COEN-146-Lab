//COEN 146L : Lab2, step 4
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
        if (argc != 2) {  // check correct usage
		fprintf(stderr, "usage: %s <n connections> \n", argv[0]);
		exit(1);
	}
        //Declare and values to n (n connections), np (np parralel connections), RTT0, RTT1, RTT2, RTTHTTP, RTTDNS, .. 
        
        
        printf("One object: %d msec\n", ?);
        printf("Non-Persistent 6 objects: %d msec\n", ?);

        //find how many np (parralel connections)
        
        printf("%d parallel connection - Persistent: %d msec\n", ?);
        printf("%d parallel connection - Non-Persistent: %d msec\n", ?);

return 0;
}