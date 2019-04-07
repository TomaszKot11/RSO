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



void square_root_handler(int, uint32_t);

void root_square_computer(int, uint64_t*, size_t );

void date_handler(int, uint32_t);

int read_wrapper(int, void*, size_t);

int write_wrapper(int, void*, size_t);

char* errno_read_handler(int);

char* errno_write_handler(int);

char* errno_accept_handler(int);

char* errno_bind_handler(int);

char* errno_listen_handler(int);

char* errno_socket_handler(int);

int main () {
   // ignore child signals
   signal (SIGCHLD, SIG_IGN);
   int server_sockfd, client_sockfd;
   socklen_t server_len, client_len;
   struct sockaddr_in server_address;
   struct sockaddr_in client_address;

   server_sockfd = socket (AF_INET, SOCK_STREAM, 0);
   if(server_sockfd == -1) {
	perror(errno_socket_handler(errno));
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
		perror(errno_bind_handler(errno));		
		exit(1);
	}

   /*  Create a connection queue, ignore child exit details and wait for clients.  */

   int listen_returned_value = listen (server_sockfd, 5);
   if(listen_returned_value == -1) {
   	perror(errno_listen_handler(errno));
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
		perror(errno_accept_handler(errno));
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
			
		   //TODO: memcpy returned value
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
				perror(errno_read_handler(errno));
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

// http://man7.org/linux/man-pages/man2/read.2.html
char* errno_read_handler(int errno_value){
	if(errno_value == EAGAIN) {
		return "File descriptior not describing the socket";
	} else if(errno_value == EAGAIN) {
		return "file descriptiot was refers to a socket and has been marked nonblocking and the read would block";
	} else if(errno_value == EBADF) {
		return "Is not vali fd or is not open for reading";
	} else if(errno_value == EFAULT) {
		return "Buffer is outside your accessible address space";
	} else if(errno_value == EINTR) {
		return "Call was interrupted by a signal before any data was read";
	} else if(errno_value == EINVAL) {
		return "fd is attached to an object which is unsuitable for reading or the wrong size bufferwas given";
	} else if(errno_value == EIO) {
		return "I/O error either a low-level or by wrong process managment";
	} else if(errno_value == EISDIR) {
		return "fd is a directory";	
	} else {
		return "Error occured while reading";
	}

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
				perror(errno_write_handler(errno));
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

// http://man7.org/linux/man-pages/man3/write.3p.html
char* errno_write_handler(int errno_value) {
	if(errno_value == EAGAIN) {
		return "The file is neihter a pipe, nor a FIFO, nor a socket";
	} else if(errno_value == EBADF) {
		return "The fd argument is not a vlaid file descriptior open for writing";
	} else if(errno_value == EFBIG) {
		return "An attempt was made to write a file that exceeds the\
              implementation-defined maximum file size or the file size\
              limit of the process, and there was no room for any bytes to\
              be written OR The file is a regular file, nbyte is greater than 0, and the\
              starting position is greater than or equal to the offset\
              maximum established in the open file description associated\
              with fd.";
	} else if(errno_value == EINTR) {
		return "The write operation was terminated due to the receipt of a\
              signal, and no data was transferred.";
	} else if(errno_value == EIO) {
		return "The process is a member of a background process group\
              attempting to write to its controlling terminal";
	} else if(errno_value == ENOSPC) {
		return "There was no free space remaining on the device containing the\
              	file.";
	} else if(errno_value == ERANGE) {
		return "The transfer request size was outside the range supported by\
              the STREAMS file associated with fildes.";	
	} else {
		return "Unknown read error occured";	
	}	
}


char* errno_socket_handler(int errno_value) {
	if(errno_value == EACCES) {
		return "Permission to create a socket of the specified type and/or protocol is denied.";
	} else if(errno_value == EAFNOSUPPORT) {
		return "The implementation does not support the specified address family.";
	} else if(errno_value == EINVAL) {
		return "Unknown protocol, or protocol family not available OR Invalid flags in type. ";
	} else if(errno_value == EMFILE) {
		return "Process file table overflow OR The system limit on the total number of open files has been reached.";
	} else if(errno_value == ENOBUFS || errno_value == ENOMEM) {
		return "Insufficient memory is available. The socket cannot be created until sufficient resources are freed. ";
	} else if(errno_value == EPROTONOSUPPORT) {
		return "The protocol type or the specified protocol is not supported within this domain.";
	} else {
		return "Unknown error occured on socket() call.";
	}	


}


char* errno_bind_handler(int errno_value) {
	if(errno_value == EACCES) {
		return "The address is protected, and the user is not the superuser.";
	} else if(errno_value == EADDRINUSE) {
		return "The given address is already in use OR (Internet domain sockets) The port number was specified as\
              zero in the socket address structure, but, upon attempting to\
              bind to an ephemeral port, it was determined that all port\
              numbers in the ephemeral port range are currently in use.";
	}  else if(errno_value == EBADF) {
		return "fd is not a valid file descriptor.";
	} else if(errno_value == EINVAL) {
		return "The socket is already bound to an address OR addrlen is wrong, or addr is not a valid address for this socket's domain.";
	} else if(errno_value == ENOTSOCK) {
		return "The file descriptor sockfd does not refer to a socket.";
	} else if(errno_value == EACCES) {
		return "Search permission is denied on a component of the path prefix.";
	} else if(errno_value == EADDRNOTAVAIL) {
		return "A nonexistent interface was requested or the requested address was not local.";
	} else if(errno_value == EFAULT) {
		return "addr points outside the user's accessible address space.";
	} else if(errno_value == ELOOP) {
		return "Too many symbolic links were encountered in resolving addr.";
	} else if(errno_value == ENAMETOOLONG) {
		return "addr is too long.";
	} else if(errno_value == ENOENT) {
		return "A component in the directory prefix of the socket pathname does not exist OR Insufficient kernel memory was available.";
	} else if(errno_value == ENOTDIR) {
		return "A component of the path prefix is not a directory.";
	} else if(errno_value == EROFS) {
		return "The socket inode would reside on a read-only filesystem.";
	} else {
		return "Unknown error occured on socket() call.";
	}	


}

char* errno_listen_handler(int errno_value) {
	if(errno_value == EADDRINUSE) {
		return "Another socket is already listening on the same port OR (Internet domain sockets) The socket referred to by sockfd had\
              not previously been bound to an address and, upon attempting\
              to bind it to an ephemeral port, it was determined that all\
              port numbers in the ephemeral port range are currently in use.";
	} else if(errno_value == EBADF) {
		return "The argument sockfd is not a valid file descriptor.";
	} else if(errno_value == ENOTSOCK) {
		return "The file descriptor sockfd does not refer to a socket.";	
	} else if(errno_value == EOPNOTSUPP) {
		return "The socket is not of a type that supports the listen() operation.";	
	} else {
		return "Unknown error occured on socket() call.";
	}	


}

char* errno_accept_handler(int errno_value) {
	if(errno_value == EAGAIN || errno_value == EWOULDBLOCK) {
		return "The socket is marked nonblocking and no connections are\
              present to be accepted.";
	} else if(errno_value == EBADF) {
		return "socketfd is not an open file descriptor.";
	} else if(errno_value == ECONNABORTED) {
		return "A connection has been aborted.";
	} else if(errno_value ==  EFAULT) {
		return "The addr argument is not in a writable part of the user\
              address space.";
	} else if(errno_value == EINTR) {
		return "The system call was interrupted by a signal that was caught\
              before a valid connection arrived.";
	} else if(errno_value == EINVAL) {
		return "Socket is not listening for connections, or addrlen is invalid\
              (e.g., is negative)";
	} else if(errno_value == EMFILE) {
		return "The per-process limit on the number of open file descriptors\
              has been reached.";
	} else if(errno_value == ENFILE) {
		return "The system-wide limit on the total number of open files has\
              been reached.";
	} else if(errno_value == ENOBUFS || errno_value == ENOMEM) {
		return "Not enough free memory.  This often means that the memory\
              allocation is limited by the socket buffer limits, not by the\
              system memory.";
	} else if(errno_value == ENOTSOCK) {
		return "The file descriptor sockfd does not refer to a socket.";
	} else if(errno_value == EOPNOTSUPP) {
		return "The referenced socket is not of type SOCK_STREAM.";
	} else if(errno_value ==  EPROTO) {
		return "";
	} else if(errno_value == EPERM) {
		return "Firewall rules forbid connection.";
	} else {
		return "Other error while establishing connection occured";
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
    uint32_t response_code = 0xFF000002ul;
    uint32_t n_response_code = htonl(response_code);
    memcpy(response_encoded, &n_response_code, sizeof(uint32_t));
	
    // code req_id	
    memcpy(response_encoded + sizeof(uint32_t), &req_id, sizeof(uint32_t)); 

    // code date length
    uint32_t date_length = sizeof(napis);
    uint32_t n_date_length = htonl(date_length);
    memcpy(response_encoded + (2 * sizeof(uint32_t)), &n_date_length, sizeof(uint32_t)); 

    // code the date
    // https://stackoverflow.com/questions/1568057/ascii-strings-and-endianness 
    // string are not affected by endianess
    memcpy(response_encoded + (3 * sizeof(uint32_t)), napis, strlen(napis));	
    write_wrapper(client_sockfd, response_encoded, ((12 + length_date) * sizeof(char)));	

    free(response_encoded);
}

