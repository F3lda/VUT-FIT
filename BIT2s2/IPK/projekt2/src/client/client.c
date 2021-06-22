/**
 * @file client.c (IPK projekt 2)
 * 
 * @brief SFTP - RFC-913 implementation
 * @date 2021-04-23
 * @author F3lda
 * @update 2021-04-24
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <fcntl.h>

/*
*   KILL THE ALL PROCESS WITH BINDED PORTS
*
* list active processes with binded ports:
*    lsof -n -i
* kill <PID>
*    ps -aux | grep "xlogin00"
*/



#define STDIN 0

#define STATIC_STRING_SIZE 512
#define MESSAGE_NULL_CHAR 1
#define SOCKET_READ_TIMEOUT 30
#define SOCKET_BLOCK_TIMEOUT 5


int LOGGED_IN = 0;
char LAST_CMD[5] = {0};
char FILE_NAME_DOWNLOAD[STATIC_STRING_SIZE] = {0};



#define INET64_ADDRSTRLEN 64

struct address_info_INET64 {
    int count;
    int current;
    int family;
    char address[INET64_ADDRSTRLEN];
    union {
        struct sockaddr_in v4;
        struct sockaddr_in6 v6;
    } addr;
};


int handleServerResponse(char * response, int bytes_received, char * request, int request_size);

int handleUserInput(char * input, char * request, int request_size, int * waiting_for_response);

int lookup_host(const char *host, struct address_info_INET64 *addr_info)
{// https://gist.github.com/jirihnidek/bf7a2363e480491da72301b228b35d5d
    struct addrinfo hints, *result, *info;
    int exit_code = 0;

    memset(&hints, 0, sizeof (hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME | AI_PASSIVE;

    if (getaddrinfo(host, NULL, &hints, &result) != 0) {
        perror("ERROR - getaddrinfo");
        return -1;
    }

    info = result;

    while(info != NULL)
    {
        if (addr_info->current == addr_info->count) {
        
            char addr_buffer[INET64_ADDRSTRLEN] = {0};
            if (info->ai_family == AF_INET) {
                addr_info->family = info->ai_family;
                if (inet_ntop(info->ai_family, &((struct sockaddr_in *)info->ai_addr)->sin_addr, addr_buffer, sizeof(addr_buffer)) == NULL) {
                    exit_code = -2;
                    break;
                }
                memcpy(addr_info->address, addr_buffer, sizeof(addr_buffer));
                memcpy(&addr_info->addr.v4, info->ai_addr, info->ai_addrlen);
                
            } else if (info->ai_family == AF_INET6) {
                addr_info->family = info->ai_family;
                if (inet_ntop(info->ai_family, &((struct sockaddr_in6 *)info->ai_addr)->sin6_addr, addr_buffer, sizeof(addr_buffer)) == NULL) {
                    exit_code = -3;
                    break;
                }
                memcpy(addr_info->address, addr_buffer, sizeof(addr_buffer));
                memcpy(&addr_info->addr.v6, info->ai_addr, info->ai_addrlen);
                
            }
            //printf("IPv%d address: %s (%s)\n", info->ai_family == PF_INET6 ? 6 : 4, addr_buffer, info->ai_canonname);
            
        }
        addr_info->count++;
        info = info->ai_next;
    }

    freeaddrinfo(result);

    return exit_code;
}

void trim(char * str)
{
    char * front = str-1;
    while(isspace(*++front));
    char * back = front+strlen(front);
    if(front[0] != 0){
        while(isspace(*--back));
        *(++back) = '\0';}
    if(front != str){memcpy(str, front, back-front+1);}
}


int main (int argc, const char * argv[])
{
    // READ ARGS
    // -------------------------
    char remote_hostname[STATIC_STRING_SIZE] = {0};
    int remote_port = 115; // default port
    
    // check args count
	if (argc != 5 && argc != 3) {
		fprintf(stderr, "usage: %s -h <hostname> {-p <port>}\n", argv[0]);
		exit(EXIT_FAILURE);
	}
    
    // parse args
    if (argc == 3) {
        
        // check arg type
        if (strcmp(argv[1], "-h") != 0) {
            fprintf(stderr, "usage: %s -h <hostname> {-p <port>}\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        // check string max size
        if(strlen(argv[2]) >= STATIC_STRING_SIZE){
            fprintf(stderr, "ERROR: some args are too big (max size: %d)\n", STATIC_STRING_SIZE-1);
            exit(EXIT_FAILURE);
        }
        // get args
        memcpy(remote_hostname, argv[2], sizeof(remote_hostname));
    
    } else {
        
        // check arg type
        if (strcmp(argv[1], "-h") == 0 && strcmp(argv[3], "-p") == 0) {
            
            // check string max size
            if(strlen(argv[2]) >= STATIC_STRING_SIZE){
                fprintf(stderr, "ERROR: some args are too big (max size: %d)\n", STATIC_STRING_SIZE-1);
                exit(EXIT_FAILURE);
            }
            // get args
            memcpy(remote_hostname, argv[2], sizeof(remote_hostname));
            remote_port = atoi(argv[4]);
            
        } else if (strcmp(argv[1], "-p") == 0 && strcmp(argv[3], "-h") == 0) {
            
            // check string max size
            if(strlen(argv[4]) >= STATIC_STRING_SIZE){
                fprintf(stderr, "ERROR: some args are too big (max size: %d)\n", STATIC_STRING_SIZE-1);
                exit(EXIT_FAILURE);
            }
            // get args
            memcpy(remote_hostname, argv[4], sizeof(remote_hostname));
            remote_port = atoi(argv[2]);
            
        } else {
            fprintf(stderr, "usage: %s -h <hostname> {-p <port>}\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    
    // check port
    if (remote_port == 0) {
        fprintf(stderr, "ERROR: invalid remote port\n");
        exit(EXIT_FAILURE);
    }
    
    
    
    
    
    // CONNECT
    // -------------------------
    
    // prepare socket
    int client_socket;
    struct address_info_INET64 addr_info;
    memset(&addr_info, 0, sizeof(addr_info));
    
    // set remote ip and family
    lookup_host(remote_hostname, &addr_info);
    
    // set remote port
    if (addr_info.family == AF_INET){
        
        //printf("IPv4\n");
        addr_info.addr.v4.sin_port = htons(remote_port);
        
    } else if (addr_info.family == AF_INET6){
        
        //printf("IPv6\n");
        addr_info.addr.v6.sin6_port = htons(remote_port);
        
    }
    
    
    
    // create socket
	if ((client_socket = socket(addr_info.family, SOCK_STREAM, 0)) < 0) {
		perror("ERROR - socket");
		exit(EXIT_FAILURE);
	}
    
    // connect socket
    if (connect(client_socket, (const struct sockaddr *)&addr_info.addr, sizeof(addr_info.addr)) < 0) {
        perror("ERROR - connect");
		exit(EXIT_FAILURE);
    }
    
    // set non-blocking socket
	int flags = fcntl(client_socket, F_GETFL, 0);
	if (fcntl(client_socket, F_SETFL, flags | O_NONBLOCK) < 0) {
		perror("ERROR - non-blocking");
		exit(EXIT_FAILURE);
	}
	
	// socket blocking timeout
	struct timeval timeoutStruct = {0*SOCKET_BLOCK_TIMEOUT, 0}; // socket blocking timeout (seconds, nanoseconds)
	fd_set readable_socket, writable_socket; // socket file descriptors
	
    
    
    
    
    // SOCKET LOOP
	// -------------------------
    char request[STATIC_STRING_SIZE] = {0};
    char response[STATIC_STRING_SIZE] = {0};
    int response_size = STATIC_STRING_SIZE-1;
    
	int bytesSent = 0;
	int bytesReceived = 0;
	
    int receiving = 0;
    int sending = 0;
    
    // check if stdin is readable
    struct timeval tv_stdin = {0L, 0L};
    fd_set fds_stdin;
    char init_request[STATIC_STRING_SIZE] = {0};
    
    int wait_for_response = 1; // set 0 or 1 (for reading commands from file)
    int waiting_for_response = 0;
    
    int done = 0;
	while(done == 0)
    {
		FD_ZERO(&readable_socket);
		FD_ZERO(&writable_socket);
		FD_SET(client_socket, &readable_socket);
		FD_SET(client_socket, &writable_socket);
		
		// check file descriptors
		select(FD_SETSIZE, &readable_socket, &writable_socket, NULL, &timeoutStruct);
		
        
        
		// ready to READ
		if (FD_ISSET(client_socket, &readable_socket)) {
			
            // check pending response in buffer
            //if ((receiving == 0 || bytesReceived == response_size) && bytesReceived == 0) {
                
                int bytesLength = recv(client_socket, &response[bytesReceived], response_size-bytesReceived, 0);
                if (bytesLength > 0) {
                    
                    // bytes received OK
                    bytesReceived += bytesLength;
                    // receiving started or continues
                    receiving = 1;
                    
                } else if (bytesLength == 0) {
                    
                    // receiving finished
                    receiving = 0;
                    
                    // proper shutdown - "end-of-file" received
                    // or full buffer
                    if(bytesReceived == response_size){
                        // RESPONSE BUFFER IS FULL
                        //printf("DONE - RESPONSE BUFFER IS FULL\n");
                        // continue reading after buffer is clear/read
                        receiving = 1;
                        
                    } else {
                        // END OF FILE -> SERVER DOWN
                        perror("ERROR - SERVER DOWN");
                        close(client_socket);
                        return -4;
                    }
                    
                } else if (bytesLength < 0) {
                    
                    // other errors
                    perror("ERROR - recv");
                    close(client_socket);
                    return -4;
                    
                }
                
            //}
            
		} else if (bytesReceived != 0 && errno != EAGAIN) {
			// reading done OK
			//printf("DONE - NOTHING TO READ\n");
            receiving = 0;
		}
        
        
        
		// ready to WRITE
		if (FD_ISSET(client_socket, &writable_socket)) {
			
            if (bytesSent != strlen(request)+MESSAGE_NULL_CHAR && request[0] != '\0') {
				
                int bytesLength = send(client_socket, &request[bytesSent], strlen(request)+MESSAGE_NULL_CHAR-bytesSent, 0);
				if (bytesLength > 0) {
					
                    // bytes sent OK
					bytesSent += bytesLength;
                    // sending started or continues
                    sending = 1;
                    
				} else if (bytesLength < 0) {
					perror("ERROR - send");
					// close client socket
					close(client_socket);
					return -5;
				} else {
                    // errors
                    //printf("NOTHING TO SEND!\n");
                }
				
			} else if (sending == 1) {
                
                // sending done
                sending = 0;
            }
		}
		
        
        
        // RESPONSE from remote (if init_request is empty)
        if ((receiving == 0 || bytesReceived == response_size) && bytesReceived != 0 && init_request[0] == '\0') {
            
            // handle response
            done = handleServerResponse(response, bytesReceived, init_request, sizeof(request));
            
            // clear response buffer
            bytesReceived = 0;
            memset(response, '\0', sizeof(response));
            
            if (receiving == 0) {
                // response received
                waiting_for_response = 0;
            }
        }
        
        
        
        // STRING from stdin (if init_request is empty)
        // info: https://stackoverflow.com/questions/448944/c-non-blocking-keyboard-input
        FD_ZERO(&fds_stdin);
        FD_SET(STDIN, &fds_stdin);
        if (select(1, &fds_stdin, NULL, NULL, &tv_stdin) > 0 && init_request[0] == '\0') {
            
            if (waiting_for_response == 0) {
                
                // read and trim line from STDIN
                char stdin_str[STATIC_STRING_SIZE] = {0};
                fgets(stdin_str, sizeof(stdin_str), stdin);
                trim(stdin_str);
                
                // skip empty lines
                if (stdin_str[0] != '\0') {
                
                    // wait for response
                    waiting_for_response = wait_for_response;
                
                    // handle input
                    handleUserInput(stdin_str, init_request, sizeof(request), &waiting_for_response);
                }
            }
        }
        
        
        
        // send prepared request
        if (sending == 0 && init_request[0] != '\0') {
            
            // copy request
            bytesSent = 0;
            memset(request, '\0', sizeof(request));
            memcpy(request, init_request, sizeof(request));
            
            // clear request buffer for a new request
            memset(init_request, '\0', sizeof(init_request));
        }
        
        
		// short delay
		usleep(1000000/30);
	}
	
	
	
	// -------------------------
	// FREE SOCKET
	// close client socket
	close(client_socket);

    return 0;
}


int handleServerResponse(char * response, int bytes_received, char * request, int request_size)
{
    //printf("RESPONSE: <%s>(%d)\n", response, bytes_received);
    //printf("LASTCMD: <%s>\n", LAST_CMD);
    //-------------------------------------------------
    
    
    
    // handle response CMD
    
    // DONE
    if (strcmp(LAST_CMD, "DONE") == 0 && strncmp(response, "+DONE", 5) == 0) {
        
        printf("CLIENT DONE.\n");
        return 1;
        
    
    // LOGIN
    } else if (strcmp(LAST_CMD, "USER") == 0 || strcmp(LAST_CMD, "ACCT") == 0 || strcmp(LAST_CMD, "PASS") == 0) {
        
        // logged in
        if (response[0] == '!') {
            LOGGED_IN = 1;
        }
        printf("%s\n", response);
    
    
    // LIST
    } else if (strcmp(LAST_CMD, "LIST") == 0) {
        
        while(strchr(response, '\n') != NULL) {
            strchr(response, '\n')[0] = ' ';
        }
        while(strchr(response, '\r') != NULL) {
            strchr(response, '\r')[1] = '\n';
            strchr(response, '\r')[0] = ' ';
        }
        
        printf("%s", response);
        
        
    // RETR
    } else if (strcmp(LAST_CMD, "RETR") == 0) {
        
        long long int numberOfBytes = 0;
        sscanf(response, " %lld", &numberOfBytes);
        if (response[0] != ' ') {
            printf("%s\n", response);
        } else {
            printf("------------------------------\n");
            printf("FILE DOWNLOAD\n");
            printf("BYTES TO DOWNLOAD: %lld\n", numberOfBytes);
            printf("FILE NAME: %s\n", FILE_NAME_DOWNLOAD);
            printf("START OF FILE\n");
            printf("------------------------------>>>");
            snprintf(request, request_size, "SEND"); // DOWNLOAD file
            strncpy(LAST_CMD, "SEND", 4);// save current CMD
        }
        
        
    // SEND
    } else if (strcmp(LAST_CMD, "SEND") == 0) {
        
        if (bytes_received+1 == request_size) {
            printf("%s", response);
        } else {
            printf("%s", response);
            printf("<<<------------------------------\n");
            printf("END OF FILE\n");
            printf("DOWNLOAD COMPLETE\n");
        }
        
        
    } else if (bytes_received+1 == request_size) {
        printf("%s", response);
    } else {
        printf("%s\n", response);
    }
    
    return 0;
}


int handleUserInput(char * input, char * request, int request_size, int * waiting_for_response)
{
    //printf("STDIN: <%s>\n", input);
    //memcpy(request, input, request_size);
    //-------------------------------------------------
    
    
    
    // check if file bytes are not on STDIN
    if (strcmp(LAST_CMD, "SIZE") != 0) { // strcmp(LAST_CMD, "SEND") != 0 && 
    
        // handle login CMDs
        // -------------------------
        if (strncmp(input, "DONE", 4) == 0) {
            
            // send CMD
            memcpy(request, input, request_size);
            
        } else if (strncmp(input, "USER ", 5) == 0) {
            
            LOGGED_IN = 0;
            
            // send CMD
            memcpy(request, input, request_size);
        
        } else if (strncmp(input, "ACCT ", 5) == 0) {
            
            LOGGED_IN = 0;
            
            // send CMD
            memcpy(request, input, request_size);
        
        } else if (strncmp(input, "PASS ", 5) == 0) {
            
            LOGGED_IN = 0;
            
            // send CMD
            memcpy(request, input, request_size);
        
        
        
        // handle CMDs after login
        // -------------------------
        } else if (LOGGED_IN == 1) {
            
            // handle after login CMDs
            // -------------------------
            if (strncmp(input, "TYPE ", 5) == 0) {
                
                // send CMD
                memcpy(request, input, request_size);
            
            } else if (strncmp(input, "LIST ", 5) == 0) {
                
                // send CMD
                memcpy(request, input, request_size);
            
            } else if (strncmp(input, "CDIR ", 5) == 0) {
                
                // send CMD
                memcpy(request, input, request_size);
            
            } else if (strncmp(input, "KILL ", 5) == 0) {
                
                // send CMD
                memcpy(request, input, request_size);
            
            } else if (strncmp(input, "NAME ", 5) == 0) {
                
                // send CMD
                memcpy(request, input, request_size);
            
            } else if (strncmp(input, "TOBE ", 5) == 0) {
                
                // send CMD
                memcpy(request, input, request_size);
            
            } else if (strncmp(input, "RETR ", 5) == 0) {
                
                // send CMD
                memcpy(request, input, request_size);
                memcpy(FILE_NAME_DOWNLOAD, input+4, sizeof(FILE_NAME_DOWNLOAD)); // save file name
            
            } else if (strncmp(input, "SEND", 4) == 0) {
                
                // send CMD
                memcpy(request, input, request_size);
            
            
            // Invalid or incomplete CMD
            // -------------------------
            } else {
                (*waiting_for_response) = 0;
                printf("Invalid or incomplete command!\n");
            }
            
        
        // Invalid or incomplete CMD
        // -------------------------
        } else {
            (*waiting_for_response) = 0;
            printf("You are not LOGGED IN or invalid or incomplete LOGIN command entered!\n");
        }
        
        // save current CMD
        strncpy(LAST_CMD, input, 4);
    
    } else {
        
        // Send file
        // -------------------------
        
        //TODO
        
        
        
        (*waiting_for_response) = 0;
        printf("Invalid or incomplete command!\n");
    }
    
    
    
    
    return 0;
}
