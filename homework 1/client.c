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


//TODO: when 3 is clicked the program stops!
// TODO: fix response code!
//TODO: use mutexes!

void print_menu();
void perform_square_root_request(int);
void perform_date_request(int);
void proceed_root_answer(int);
void proceed_date_answer(int);
// function that is run by the thread
void* incomming_request_handler(void*);

// this variables are modified only by one thread
// that's why there are thread-safe
int root_request_counter = 0;
int date_request_counter = 0;
int should_run = 1;
int sockfd;


int main () {
   int request_no = 0;
   
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

	// run incomming request hanlder
	pthread_t thread_id;
	int error = pthread_create(&thread_id, NULL, incomming_request_handler, NULL);
	if(error != 0) {
		printf("\nThread can;t be created!\n");
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


// pthread_cleanup_push to do the necessary cleanups - in our
// case there is no cleanupss
void* incomming_request_handler(void* vargp) {
	//printf("Thread receiving responses started!");
	while(should_run) {
		uint32_t answer_code, request_id;
		uint32_t h_answer_code, h_request_id;
        	// decode the request_code
		ssize_t no_bytes = read(sockfd, &answer_code, sizeof(uint32_t));
		if(no_bytes < 0) {
			printf("[ANSWER_CODE] Wrong number of bytes were read!");
			exit(1);	
		}

		h_answer_code = ntohl(answer_code);
		no_bytes = read(sockfd, &request_id, sizeof(uint32_t));
		if(no_bytes < 0) {
			printf("[REQUEST_ID] Wrong number of bytes were read!");
			exit(1);	
		}
	
		// decode the reuqest_id
		h_request_id = ntohl(request_id);


	
		switch(h_answer_code) {
			case 0xFF0000FFul:
				// root answer 
				if(root_request_counter < h_request_id) {
					printf("Wrong request id!");
					exit(1);
				}
				proceed_root_answer(sockfd);			
				break;
			case 0xFF000002ul:
				// date answer 
				if(date_request_counter < h_request_id) {
					printf("Wrong request id!");
					exit(1);
				}
				proceed_date_answer(sockfd);
				break;
			default:
				printf("Wrong answer code!");
				exit(1);
		}

	}
	//printf("Thread receiving responses ended!");
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

	ssize_t no_bytes = read(sockfd, &data_length, sizeof(uint32_t));
	if(no_bytes < 0) {
		printf("[PROCEED_DATE_REQUEST] Error while reading data!");
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
		printf("[PROCEED_DATE_REQUEST] Error while writing data!");
		exit(1);	
	}
	
	printf("\nDate received: %s\n", date);

	free(date);

}

void proceed_root_answer(int sockfd) {
	char request_coded[8];


	memset(request_coded, '\0', 8 * sizeof(char));

	ssize_t no_bytes = read(sockfd, request_coded, 8 * sizeof(char));
	if(no_bytes < 0) {
		printf("Error reading data!");
		exit(1);	
	}

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
	uint32_t request_id = 1;
	char request_coded[16];
	// code for network transfer
	uint32_t n_request_code = htonl(request_code);
	uint32_t n_request_id = htonl(request_id);

	memcpy(request_coded, &n_request_code, sizeof(n_request_code));
	memcpy(request_coded + sizeof(n_request_code), &n_request_id , sizeof(n_request_id));
	memcpy(request_coded + (sizeof(n_request_code) + sizeof(n_request_id)), &n_my_digit_bits, 		sizeof(n_my_digit_bits));

	ssize_t no_bytes = write(sockfd, request_coded, 16 * sizeof(char));
	if(no_bytes == -1) {
		printf("Error while writing data!");
		exit(1);
	}
	root_request_counter++;
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


	date_request_counter++;
}	








