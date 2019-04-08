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
#include <time.h>
#include <locale.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define REQ_LENGTH 8
#define ROOT_ANSWER_CODE 0x1000001
#define DATE_ANSER_CODE 0x10002

void square_root_handler(int, uint32_t);

void root_square_computer(int, uint64_t*, size_t );

void date_handler(int, uint32_t);

int read_wrapper(int, void*, size_t);

int write_wrapper(int, void*, size_t);

int main () {
   // ignore child signals
   signal (SIGCHLD, SIG_IGN);
   int server_sockfd, client_sockfd;
   socklen_t server_len, client_len;
   struct sockaddr_in server_address;
   struct sockaddr_in client_address;

   server_sockfd = socket (AF_INET, SOCK_STREAM, 0);
   if(server_sockfd == -1) {
	perror(strerror(errno));
	exit(1);
   }	

   server_address.sin_family = AF_INET;
   // htonl(INADDR_LOOPBACK);
   //htonl(INADDR_ANY);
   server_address.sin_addr.s_addr = htonl(INADDR_ANY);
   server_address.sin_port = htons (9734);
   server_len = sizeof (server_address);

   int bind_returned_value = bind (server_sockfd, (struct sockaddr *) &server_address, server_len);

	if(bind_returned_value == -1) {
		perror(strerror(errno));		
		exit(1);
	}

   /*  Create a connection queue, ignore child exit details and wait for clients.  */

   int listen_returned_value = listen (server_sockfd, 5);
   if(listen_returned_value == -1) {
   	perror(strerror(errno));
	exit(1);
    }
   // signal (SIGCHLD, SIG_IGN);

   while (1) {

       printf ("Server waiting for next connection!\n");

       /*  Accept connection.  */

       client_len = sizeof (client_address);
       client_sockfd = accept (server_sockfd,
               (struct sockaddr *) &client_address,
               &client_len);
	// error handling
	if(client_sockfd == -1) {
		perror(strerror(errno));
	}

	printf("New connection!\n");

       /*  Fork to create a process for this client and perform a test to see
           whether we're the parent or the child.  */

       if (fork () == 0) {
		while(1) {
		   // the children
		  uint64_t selector_req_id_buffer;
		  read_wrapper(client_sockfd, &selector_req_id_buffer, sizeof(uint64_t));
	          uint32_t selector_l, req_id; 
		  // copy the memory from the buffer
		
		   memcpy(&selector_l, &selector_req_id_buffer, sizeof(uint32_t));
		   memcpy(&req_id, &selector_req_id_buffer + sizeof(uint32_t), sizeof(uint32_t));		
		   selector_l = ntohl(selector_l);

		   switch(selector_l) {
		       case 0xFFul:
			    // process the square roto request
		       	    square_root_handler(client_sockfd, req_id);
			    break;
			case 0x2ul:
			    //printf("Received the date request!\n");
			    date_handler(client_sockfd, req_id);
			    break;
		       default:
		           printf("Wrong request\n");
		           exit(1);
		   }
		}
       } else {
           close (client_sockfd);
       }
   }
}

// https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/rtrea.htm
int read_wrapper(int socketfd, void* buffer, size_t size){
	size_t total_read = 0, total_left = size;
	char* buffer_pointer = (char*)buffer;

	while(total_left > 0) {
		size_t current_read = read(socketfd, buffer_pointer, total_left); 
		
		if(current_read <= 0){
			// end of the file reached
			// peer called close() function			
			if(current_read == 0) {
				perror("End of the file reached!");
				close(socketfd);
				exit(1);
			} else if(current_read < 0) {
				perror(strerror(errno));
				close(socketfd);
				exit(1);			
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
		size_t current_written = write(socketfd, buffer_pointer, total_left);
		
 
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

void square_root_handler(int client_sockfd, uint32_t req_id) {
	    // process the root request
	    uint64_t result;
	    root_square_computer(client_sockfd, &result, sizeof(uint64_t));
	    char result_coded[16];
            // code the result
	    uint32_t root_code_result = htonl(ROOT_ANSWER_CODE);
	

            memcpy(result_coded, &root_code_result, sizeof(uint32_t));
	    memcpy(result_coded + sizeof(uint32_t), &req_id, sizeof(uint32_t));
	    memcpy(result_coded + (2 * sizeof(uint32_t)), &result, sizeof(uint64_t)); 
	    write_wrapper(client_sockfd, result_coded, 16 * sizeof(char));	
}



void root_square_computer(int client_socket, uint64_t* result, size_t size) {

   // read the double value from socket connection
   uint64_t number;
   read_wrapper(client_socket, &number, sizeof(uint64_t));	

   uint64_t value_for_host = be64toh(number);
   double value = 0.0;
   memcpy(&value, &value_for_host, sizeof(value_for_host));

   double d_result = sqrt(value);

   uint64_t output_value;
   memcpy(&output_value, &d_result, sizeof(uint64_t));


   uint64_t network_result = htobe64(output_value);
   memcpy(result, &network_result, size);

}


void date_handler(int client_sockfd, uint32_t req_id) {
	

    time_t sekund;
    struct tm *wsk_strukt;
    char napis[60];

    setlocale (LC_ALL, "pl_PL");  

    time (&sekund);
    wsk_strukt = localtime (&sekund);
    strftime (napis, 60, "%c", wsk_strukt);


    uint32_t length_date = strlen(napis);
    // encode the response
    char* response_encoded = malloc(12+length_date);
    if(response_encoded == NULL) {
    	printf("Error allocating memory!");
	exit(1); 
    }
    // request code
    uint32_t response_code = DATE_ANSER_CODE;
    uint32_t n_response_code = htonl(response_code);
    memcpy(response_encoded, &n_response_code, sizeof(uint32_t));
	
    // code req_id	
    memcpy(response_encoded + sizeof(uint32_t), &req_id, sizeof(uint32_t)); 

    // code date length
    uint32_t date_length = strlen(napis);
    uint32_t n_date_length = htonl(date_length);
    memcpy(response_encoded + (2 * sizeof(uint32_t)), &n_date_length, sizeof(uint32_t)); 

    // code the date
    // https://stackoverflow.com/questions/1568057/ascii-strings-and-endianness 
    // string are not affected by endianess
    memcpy(response_encoded + (3 * sizeof(uint32_t)), napis, strlen(napis));	
    write_wrapper(client_sockfd, response_encoded, ((12 + length_date) * sizeof(char)));	

    free(response_encoded);
}

