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
#include <pthread.h>
#include <signal.h>
#include <errno.h>

void print_menu();
void perform_square_root_request(int);
void perform_date_request(int);
void proceed_root_answer(int);
void proceed_date_answer(int);
// function that is run by the thread
void* incomming_request_handler(void*);

int read_wrapper(int, void*, size_t);

int write_wrapper(int, void*, size_t);

// this variables are modified only by one thread
// that's why there are thread-safe
int root_request_counter = 0;
int date_request_counter = 0;
int should_run = 1;
int sockfd;

// https://developer.ibm.com/tutorials/l-sockpit/
int main () {
   int request_no = 0;
   
   socklen_t len;
   struct sockaddr_in address;
   int result;

   sockfd = socket (AF_INET, SOCK_STREAM, 0);
   if(sockfd == -1) {
	perror(strerror(errno));	
	exit(1);
   }	

   address.sin_family = AF_INET;
   address.sin_addr.s_addr = inet_addr ("127.0.0.1");
   address.sin_port = htons (9734);
   len = sizeof (address);

   result = connect (sockfd, (struct sockaddr *) &address, len);
   if (result == -1) {
       perror (strerror(errno));
       exit (1);
   }

	// run incomming request hanlder
	pthread_t thread_id;
	int error = pthread_create(&thread_id, NULL, incomming_request_handler, NULL);
	if(error != 0) {
		perror("\nThread can;t be created!\n");
		exit(1);
	}

	while(should_run) {
		do {	    
		    print_menu();
		    printf("Your option: ");
     	            scanf("%d", &request_no);

			switch(request_no) {
			  case 1:
				perform_square_root_request(sockfd);
				break;
			  case 2:
				perform_date_request(sockfd);
				break;
			  case 3:
				should_run = 0;
			     	break;
			}
		} while(request_no < 1 && request_no > 3);
		
	}
   //pthread_join(thread_id, NULL);	
   //int returned_value = pthread_kill(thread_id, SIGKILL);
   //if(returned_value > 0) printf("Error closing the thread!");
   int returned_value = pthread_cancel(thread_id);
  // pthread_setcancelstate
   if(returned_value > 0) fprintf(stderr, "Error closing the thread!");
   close (sockfd);
   printf("\nBYE!\n");
   exit (0);
}
// https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/rtrea.htm
int read_wrapper(int socketfd, void* buffer, size_t size){
	size_t total_read = 0, total_left = size;
	char* buffer_pointer = (char*)buffer;

	while(total_left > 0) {
		ssize_t current_read = read(socketfd, buffer_pointer, total_left); 
		
		if(current_read <= 0){
			// end of the file reached
			// peer called close() function			
			if(current_read == 0) {
				perror("End of the file reached!");
				close(socketfd);
				return 0;
				//exit(1);
				
			} else if(current_read < 0) {
				// EINTR the call was interrupted before any data was read
				if(errno == EINTR) continue;
				perror(strerror(errno));
				close(socketfd);
				return -1;
				//exit(1);			
			}

		} else {
			total_read += current_read;
			total_left -= current_read;
			buffer_pointer += current_read;
		}
	}
	
	
	return total_read;
}

int write_wrapper(int socketfd, void* buffer, size_t size) {
	size_t total_written = 0, total_left = size; 
	char* buffer_pointer = (char*)buffer;
	
	while(total_left > 0) {
		ssize_t current_written = write(socketfd, buffer_pointer, total_left);
		
 
		if(current_written <= 0) {

			if(current_written == 0) {
				perror("End of the file reached!");
				close(socketfd);
				exit(1);
			} else if(current_written < 0) {
				perror(strerror(errno));
				close(socketfd);
				exit(1);			
			}
		} else { 
			total_written += current_written;
			total_left -= current_written; 
			buffer_pointer += current_written;		
		}
		
	}

	return total_written;
}

// pthread_cleanup_push to do the necessary cleanups - in our
// case there is no cleanupss
void* incomming_request_handler(void* vargp) {
	//printf("Thread receiving responses started!");
	while(should_run) {
		uint32_t h_answer_code, h_request_id;
		uint64_t answer_code_request_id;
        	
		read_wrapper(sockfd, &answer_code_request_id, sizeof(uint64_t));
		
		memcpy(&h_answer_code, &answer_code_request_id, sizeof(uint32_t));
		memcpy(&h_request_id, (char*)&answer_code_request_id + sizeof(uint32_t), sizeof(uint32_t));
	
	
		// decode the reuqest_id

		h_answer_code = ntohl(h_answer_code);
		h_request_id = ntohl(h_request_id);

	
		switch(h_answer_code) {
			case 0xFF0000FFul:
				// root answer 
				if(root_request_counter < h_request_id) {
					perror("Wrong request id!");
					exit(1);
				}
				proceed_root_answer(sockfd);			
				break;
			case 0xFF000002ul:
				// date answer 
				if(date_request_counter < h_request_id) {
					perror("Wrong request id!");
					exit(1);
				}
				proceed_date_answer(sockfd);
				break;
			default:
				perror("Wrong answer code!");
				exit(1);
		}

	}
}


void print_menu() {
	printf("Select option\n");
	printf("1 - square root\n");
	printf("2 - current date\n");
	printf("3 - close connection\n"); 
}


void proceed_date_answer(int sockfd) { 
	char request_coded[8];

	uint32_t req_code;
	uint32_t req_id;
	uint32_t data_length;

	read_wrapper(sockfd, &data_length, sizeof(uint32_t));
	
	uint32_t h_data_length = ntohl(data_length);		
	
	char* date = malloc((h_data_length * sizeof(char)) + 1);
	if(date == NULL) {
		perror("Error allocating memory!");
		exit(1);
	}

	 	
	read_wrapper(sockfd, date, h_data_length * sizeof(char));

	*(date + h_data_length) = '\0';
	
	printf("\nDate received: %s\n", date);

	free(date);

}

void proceed_root_answer(int sockfd) {
	char request_coded[8];


	memset(request_coded, '\0', 8 * sizeof(char));
	read_wrapper(sockfd, request_coded, 8 * sizeof(char));

	uint64_t result;
	memcpy(&result, request_coded, sizeof(result));
	uint64_t h_result = be64toh(result);

	double d_result;
	memcpy(&d_result, &h_result, sizeof(d_result));
	printf("\n The result is: %lf\n", d_result);
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
	uint32_t request_id = root_request_counter;
	char request_coded[16];
	// code for network transfer
	uint32_t n_request_code = htonl(request_code);
	uint32_t n_request_id = htonl(request_id);

	memcpy(request_coded, &n_request_code, sizeof(n_request_code));
	memcpy(request_coded + sizeof(n_request_code), &n_request_id , sizeof(n_request_id));
	memcpy(request_coded + (sizeof(n_request_code) + sizeof(n_request_id)), &n_my_digit_bits, sizeof(n_my_digit_bits));

	write_wrapper(sockfd, request_coded, 16 * sizeof(char));
	root_request_counter++;
}



void perform_date_request(int sockfd) {
	uint32_t request_code = 2;
	uint32_t request_id = date_request_counter;
	char request_coded[8];
	// code for network transfer
	uint32_t n_request_code = htonl(request_code);
	uint32_t n_request_id = htonl(request_id);

	memcpy(request_coded, &n_request_code, sizeof(n_request_code));
	memcpy(request_coded + sizeof(n_request_code), &n_request_id , sizeof(n_request_id));

	write_wrapper(sockfd, request_coded, 8 * sizeof(char));


	date_request_counter++;
}	








