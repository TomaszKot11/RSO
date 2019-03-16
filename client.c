/*  Make the necessary includes and set up the variables.  */
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <endian.h>   
#include <byteswap.h>



void print_menu() {
	printf("Select option");
	printf("1 - square root");
	printf("2 - current date");
	printf("q - close connection"); 
}



int main ()
{
   int sockfd;
   socklen_t len;
   struct sockaddr_in address;
   int result;

   /*  Create a socket for the client.  */

   sockfd = socket (AF_INET, SOCK_STREAM, 0);

   /*  Name the socket, as agreed with the server.  */

   address.sin_family = AF_INET;
   address.sin_addr.s_addr = inet_addr ("127.0.0.1");
   address.sin_port = htons (9734);
   len = sizeof (address);

   /*  Now connect our socket to the server's socket.  */

   result = connect (sockfd, (struct sockaddr *) &address, len);

   if (result == -1) {
       perror ("Error while connectiong!");
       exit (1);
   }
	
	while(1) {
		double my_digit = 0;
		printf("Please give me a digit:");
		scanf("%lf", &my_digit);
		uint64_t my_digit_bits;
		memcpy(&my_digit_bits, &my_digit, sizeof(my_digit));
		// dotad ok
		printf("%ld\n", my_digit_bits);


		uint64_t n_my_digit_bits = htobe64(my_digit_bits);


		// encode root request
		uint32_t request_code = 255;
		uint32_t request_id = 1;
		char request_coded[16];
		// code for network transfer
		uint32_t n_request_code = htonl(request_code);
		uint32_t n_request_id = htonl(request_id);

		memcpy(request_coded, &n_request_code, sizeof(n_request_code));
		memcpy(request_coded + sizeof(n_request_code), &n_request_id , sizeof(n_request_id));
		memcpy(request_coded + (sizeof(n_request_code) + sizeof(n_request_id)), &n_my_digit_bits, sizeof(n_my_digit_bits));


	
		write(sockfd, request_coded, 16 * sizeof(char));

		memset(request_coded, '\0', 16 * sizeof(char));

		read(sockfd, request_coded, 16 * sizeof(char));

		uint64_t result;
		memcpy(&result, request_coded + (8 * sizeof(char)), sizeof(result));
		uint64_t h_result = be64toh(result);
		printf("\nThe result is: %ld\n", result);
		double d_result;
		memcpy(&d_result, &h_result, sizeof(d_result));
		printf("\n The result is: %lf\n", d_result);

		// decode the answear
	}	
   close (sockfd);
   exit (0);
}







