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


//TODO: request_id!

void print_menu();
void perform_square_root_request(int);
void perform_date_request(int);


int main () {
   int should_run = 1;
   int sockfd;
   socklen_t len;
   struct sockaddr_in address;
   int result;

   sockfd = socket (AF_INET, SOCK_STREAM, 0);

   address.sin_family = AF_INET;
   address.sin_addr.s_addr = inet_addr ("127.0.0.1");
   address.sin_port = htons (9734);
   len = sizeof (address);

   result = connect (sockfd, (struct sockaddr *) &address, len);
   if (result == -1) {
       perror ("Error while connectiong!");
       exit (1);
   }
	
	while(should_run) {
	        int request_no = 0;
		do {	    
		    print_menu();
		    printf("Your option: ");
     	            scanf("%d", &request_no);
		  } while(request_no < 1 && request_no > 3);
		

		switch(request_no) {
	          case 1:
			perform_date_request(sockfd);
			break;
		  case 2:
			perform_square_root_request(sockfd);
			break;
		  case 3:
			should_run = 0;
		       break;
		  default:
			printf("\nNo such option!\n");
		}
		//TODO: decode the answer and check req_id
	}	
   close (sockfd);
   exit (0);
}


void print_menu() {
	printf("Select option\n");
	printf("1 - square root\n");
	printf("2 - current date\n");
	printf("3 - close connection\n"); 
}

void perform_square_root_request(int sockfd) {
	double my_digit = 0;
	printf("Please insert your digit:");
	scanf("%lf", &my_digit);
	uint64_t my_digit_bits;
	memcpy(&my_digit_bits, &my_digit, sizeof(my_digit));


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

	ssize_t no_bytes = write(sockfd, request_coded, 16 * sizeof(char));
	if(no_bytes == -1) {
		printf("Error while writing data!");
		exit(1);
	}

	memset(request_coded, '\0', 16 * sizeof(char));

	no_bytes = read(sockfd, request_coded, 16 * sizeof(char));
	if(no_bytes < 0) {
		printf("Error reading data!");
		exit(1);	
	}

	uint64_t result;
	memcpy(&result, request_coded + (8 * sizeof(char)), sizeof(result));
	uint64_t h_result = be64toh(result);

	double d_result;
	memcpy(&d_result, &h_result, sizeof(d_result));
	printf("\n The result is: %lf\n", d_result);
}



void perform_date_request(int sockfd) {
	uint32_t request_code = 2;
	uint32_t request_id = 1;
	char request_coded[8];
	// code for network transfer
	uint32_t n_request_code = htonl(request_code);
	uint32_t n_request_id = htonl(request_id);

	memcpy(request_coded, &n_request_code, sizeof(n_request_code));
	memcpy(request_coded + sizeof(n_request_code), &n_request_id , sizeof(n_request_id));
	
	ssize_t no_bytes = write(sockfd, request_coded, 8 * sizeof(char));
	if(no_bytes == -1) {
		printf("[PERFORM_DATE_REQUEST] Error while writing data!");
		exit(1);
	}


	// read the response!

	// read first 12 bytes to get the legth of the string
	uint32_t req_code;
	uint32_t req_id;
	uint32_t data_length;

	no_bytes = read(sockfd, &req_code, sizeof(uint32_t));
	if(no_bytes < 0) {
		printf("[PERFORM_DATE_REQUEST] Error while reading data!");
		exit(1);
	}
	
	no_bytes = read(sockfd, &req_id, sizeof(uint32_t));
	if(no_bytes < 0) {
		printf("[PERFORM_DATE_REQUEST] Error while reading data!");
		exit(1);
	}
	
	no_bytes = read(sockfd, &data_length, sizeof(uint32_t));
	if(no_bytes < 0) {
		printf("[PERFORM_DATE_REQUEST] Error while reading data!");
		exit(1);
	}
	
	uint32_t h_data_length = ntohl(data_length);		
	
	char* date = malloc(h_data_length * sizeof(char));
	if(date == NULL) {
		printf("Error allocating memory!");
		exit(1);
	}

	no_bytes = read(sockfd, date, h_data_length * sizeof(char));
	if(no_bytes < 0) {
		printf("[PERFORM_DATE_REQUEST] Error while writing data!");
		exit(1);	
	}
	
	printf("\nDate received: %s\n", date);

	free(date);
}	








