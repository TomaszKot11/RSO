#include <string.h> /* memcpy */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* close */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <math.h>
#include <endian.h>    // __BYTE_ORDER __LITTLE_ENDIAN
#include <byteswap.h>  // bswap_64()


#define BUFFER_SIZE 1024
#define REQ_LENGTH 8


//TODO: ignorowanie sygnałów


void square_root_handler(int, uint32_t);

void root_square_computer(int, uint64_t*, size_t );



int main () {
   int server_sockfd, client_sockfd;
   socklen_t server_len, client_len;
   struct sockaddr_in server_address;
   struct sockaddr_in client_address;

   server_sockfd = socket (AF_INET, SOCK_STREAM, 0);

   server_address.sin_family = AF_INET;
   // htonl(INADDR_LOOPBACK);
   //htonl(INADDR_ANY);
   server_address.sin_addr.s_addr = htonl(INADDR_ANY);
   server_address.sin_port = htons (9734);
   server_len = sizeof (server_address);
   bind (server_sockfd, (struct sockaddr *) &server_address, server_len);

   /*  Create a connection queue, ignore child exit details and wait for clients.  */

   listen (server_sockfd, 5);

   // signal (SIGCHLD, SIG_IGN);

   while (1) {

       printf ("server waiting for next connection!\n");

       /*  Accept connection.  */

       client_len = sizeof (client_address);
       client_sockfd = accept (server_sockfd,
               (struct sockaddr *) &client_address,
               &client_len);

	printf("New connection!");

       /*  Fork to create a process for this client and perform a test to see
           whether we're the parent or the child.  */

       if (fork () == 0) {
		while(1) {
		   // the children
		   uint32_t buffer;
		   uint32_t buffer_two;
	    
		   // which operation to perform
		   ssize_t no_bytes = read(client_sockfd, &buffer, sizeof(uint32_t));
		   if(no_bytes < sizeof(uint32_t)) {
			printf("Wrong number of bytes was read!");
			exit(1);
	   	   }


		  no_bytes = read(client_sockfd, &buffer_two, sizeof(uint32_t));
		  if(no_bytes < sizeof(uint32_t)) {
		       printf("Wrong number of bytes was read!");
		       exit(1);
		   }

		   uint32_t selector_l = ntohl(buffer);
		   uint32_t req_id = buffer_two;

		   switch(selector_l) {
		       case 0xFFul:
			 {
		       	    square_root_handler(client_sockfd, req_id);
			 }
			break;
		       //case 0xFF0000FFul:
		           // process the time request

		       default:
		           printf("Wrong request");
		           exit(1);
		   }
		}
       } else {
           close (client_sockfd);
       }
   }
}


void square_root_handler(int client_sockfd, uint32_t req_id) {
	    // process the root request
	    uint64_t result;
	    root_square_computer(client_sockfd, &result, sizeof(uint64_t));
	    char result_coded[16];
            // code the result
	    uint32_t root_code_result = htonl(0xFF0000FFul);
            memcpy(result_coded, &root_code_result, sizeof(uint32_t));
	    memcpy(result_coded + sizeof(uint32_t), &req_id, sizeof(uint32_t));
	    memcpy(result_coded + (2 * sizeof(uint32_t)), &result, sizeof(uint64_t)); 
			
	    ssize_t no_bytes_two = write(client_sockfd, result_coded, 16 * sizeof(char));
	    if(no_bytes_two < 16 * sizeof(char)) {
		 printf("Wrong number of bytes was written!");
		 exit(1);
	    }
}



void root_square_computer(int client_socket, uint64_t* result, size_t size) {

   // read the double value from socket connection
   uint64_t number;
   ssize_t no_bytes = read(client_socket, &number, sizeof(uint64_t));
   if(no_bytes < sizeof(uint64_t)) {
     printf("[In root handler] Wrong number of bytes was read!");
	exit(1);
   }

   uint64_t value_for_host = be64toh(number);
   double value = 0.0;
   memcpy(&value, &value_for_host, sizeof(value_for_host));
   double d_result = sqrt(value);

   uint64_t output_value;
   memcpy(&output_value, &d_result, sizeof(uint64_t));


   uint64_t network_result = htobe64(output_value);
   memcpy(result, &network_result, size);

}


