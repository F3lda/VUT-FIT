/**
 * @file fileget.c (IPK projekt 1)
 * 
 * @brief Trivial distributed file system
 * @date 2021-03-25
 * @author F3lda
 * @update 2021-03-30
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>


#define STATIC_STRING_SIZE 512
#define SOCKET_READ_TIMEOUT 30
#define SOCKET_BLOCK_TIMEOUT 5
#define SOCKET_FILE_BUFFER_SIZE 262144


/* FUNCTIONS */
void downloadFile(char * file_server_name, char * file_server_ip, int file_server_port, char * file_url);

/* CUSTOM FUNCTIONS */
void trim(char * str);
void strtolower(char *str);

/* NETWORKING FUNCTIONS */
int UDPrequest(char * server_ip, int server_port, char * request, char * response, int response_size);
int TCPrequest(char * server_ip, int server_port, char * request, char * response, int response_size);
int TCPrequestHeaders(char * server_ip, int server_port, char * request, char * response, int response_size);
int TCPsocketSaveFile(int client_socket, unsigned int buffer_size, char * filename);


/****************************************************** MAIN ******************************************************************************/

int main(int argc, const char * argv[])
{
	/* -------------------------------- ARGS ----------------------------------- */
	// -------------------------
	// READ ARGS
	char name_server_ip[STATIC_STRING_SIZE] = {0};
	int name_server_port = 0;
	
	char file_url[STATIC_STRING_SIZE] = {0};
	char file_server_name[STATIC_STRING_SIZE] = {0};
	
	char file_server_ip[STATIC_STRING_SIZE] = {0};
	int file_server_port = 0;
	
	
	// check args count
	if (argc != 5) {
		fprintf(stderr,"usage: %s -n <name_server> -f <fsp_file_url>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	// check string max buffer size
	if(strlen(argv[2]) >= STATIC_STRING_SIZE || strlen(argv[4]) >= STATIC_STRING_SIZE){
		fprintf(stderr,"ERROR: some args are too big (max chars: %d)\n", STATIC_STRING_SIZE-1);
		exit(EXIT_FAILURE);
	}
	// check args types
	if(strcmp(argv[1], "-n") == 0 && strcmp(argv[3], "-f") == 0){
		memcpy(name_server_ip, argv[2], STATIC_STRING_SIZE-1);
		memcpy(file_url, argv[4], STATIC_STRING_SIZE-1);
	} else if(strcmp(argv[1], "-f") == 0 && strcmp(argv[3], "-n") == 0){
		memcpy(file_url, argv[2], STATIC_STRING_SIZE-1);
		memcpy(name_server_ip, argv[4], STATIC_STRING_SIZE-1);
	} else {
		fprintf(stderr,"usage: %s -n <name_server> -f <fsp_file_url>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	//printf("%s\n", name_server_ip);
	//printf("%s\n", file_url);
	
	
	
	// -------------------------
	// PARSE ARGS
	// parse name server ip and port
	char *port_ptr = strstr(name_server_ip, ":");
	if(port_ptr != NULL){
		port_ptr[0] = '\0';
		port_ptr++;
		name_server_port = atoi(port_ptr);
	} else {
		name_server_port = 53;
	}
	
	// parse file server name and file url
	if(strstr(file_url, "fsp://") != NULL && strstr(file_url, "fsp://") == file_url){
		char *file_server_name_ptr = &file_url[6];
		char *file_url_ptr = strstr(file_server_name_ptr, "/");
		if(file_url_ptr != NULL){
			file_url_ptr[0] = '\0';
			file_url_ptr++;
			strcpy(file_server_name, file_server_name_ptr);
			strtolower(file_server_name);
			strcpy(file_url, file_url_ptr);
		}
	}
	if(file_server_name[0] == '\0'){
		fprintf(stderr,"ERROR: invalid file_url\n");
		exit(EXIT_FAILURE);
	}
	
	
	
	// -------------------------
	// CHECK ARGS TYPES
	int i;
	// check server ip -> numbers and dots
	for(i = 0; i < strlen(name_server_ip); i++){
		if(!(('0' <= name_server_ip[i] && name_server_ip[i] <= '9') || name_server_ip[i] == '.')){
			fprintf(stderr,"ERROR: ivalid name_server_ip\n");
			exit(EXIT_FAILURE);
		}
	}
	// check port number -> not zero
	if(name_server_port == 0){
		fprintf(stderr,"ERROR: invalid name_server_port\n");
		exit(EXIT_FAILURE);
	}
	// check file server name -> "-", "_" a "." and alphanumeric
	for(i = 0; i < strlen(file_server_name); i++){
		if(!(('0' <= file_server_name[i] && file_server_name[i] <= '9') || ('a' <= file_server_name[i] && file_server_name[i] <= 'z') || file_server_name[i] == '.' || file_server_name[i] == '-' || file_server_name[i] == '_')){
			fprintf(stderr,"ERROR: ivalid file_server_name\n");
			exit(EXIT_FAILURE);
		}
	}
	// check file url -> "-", "_", "." and "/" and alphanumeric
	for(i = 0; i < strlen(file_url); i++){
		if(!(('0' <= file_url[i] && file_url[i] <= '9') || ('a' <= file_url[i] && file_url[i] <= 'z') || file_url[i] == '.' || file_url[i] == '-' || file_url[i] == '_' || file_url[i] == '/')){
			if(file_url[0] != '*' && file_url[1] != '\0'){
				fprintf(stderr,"ERROR: ivalid file_url\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	
	printf("\n------------------------------ INFO ------------------------------\n");
	
	printf("NAME SERVER IP: %s\n", name_server_ip);
	printf("NAME SERVER PORT: %d\n", name_server_port);
	
	printf("FILE SERVER: %s\n", file_server_name);
	printf("FILE URL: %s\n", file_url);
	
	
	
	
	
	/* -------------------------------- DNS ----------------------------------- */
	printf("\n------------------------------ DNS ------------------------------\n");
	
	char strRequest[STATIC_STRING_SIZE] = "WHEREIS ";
	memcpy(&strRequest[strlen(strRequest)], file_server_name, STATIC_STRING_SIZE-1-strlen(strRequest));
	char strResponse[STATIC_STRING_SIZE] = {0};
	
	printf("REQUEST: %s\n", strRequest);
	printf("----------\n");
	if(0 != UDPrequest(name_server_ip, name_server_port, strRequest, strResponse, STATIC_STRING_SIZE-1)){
		exit(EXIT_FAILURE);
	}
	printf("----------\n");
	printf("RESPONSE: %s\n", strResponse);
	if(strResponse[0] != 'O' && strResponse[1] != 'K'){
		fprintf(stderr,"DNS ERROR: %s\n", strResponse);
		exit(EXIT_FAILURE);
	}
	
	
	
	
	
	/* -------------------------------- DOWNLOAD ----------------------------------- */
	// -------------------------
	// PARSE FILE SERVER IP AND PORT
	strstr(&strResponse[3],":")[0] = ' ';
	sscanf(&strResponse[3],"%s %d", file_server_ip, &file_server_port);
	printf("\n------------------------------ DOWNLOAD ------------------------------\n");
	printf("FILE SERVER IP: %s\n", file_server_ip);
	printf("FILE SERVER PORT: %d\n", file_server_port);
	
	
	
	// -------------------------
	// DOWNLOAD JUST ONE FILE
	if(file_url[0] != '*'){
		printf("DOWNLOAD TYPE: ONE FILE\n");
		downloadFile(file_server_name, file_server_ip, file_server_port, file_url);
		printf("<%s>\n", file_url);
		
		
		
	// -------------------------
	// DOWNLOAD ALL FILES
	} else {
		printf("DOWNLOAD TYPE: ALL FILES\n");
		downloadFile(file_server_name, file_server_ip, file_server_port, "index");
		FILE *index = fopen("index", "r");
		if(index == NULL){
			fprintf(stderr,"ERROR - OPEN FILE: index\n");
			exit(EXIT_FAILURE);
		}
		
		char buffer[STATIC_STRING_SIZE] = {0};
		while(fgets(buffer, STATIC_STRING_SIZE-1, index)) {
			trim(buffer);
			if(buffer[0] != '\0'){
				downloadFile(file_server_name, file_server_ip, file_server_port, buffer);
				printf("<%s>\n", buffer);
			}
		}
		
		fclose(index);
	}
	
	
	
	printf("\n------------------------------ DONE ------------------------------\n");
	return 0;
}



/****************************************************** FUNCTIONS ******************************************************************************/

void downloadFile(char * file_server_name, char * file_server_ip, int file_server_port, char * file_url)
{
	// create request string
	char strRequest[STATIC_STRING_SIZE] = "GET ";
	memcpy(&strRequest[strlen(strRequest)], file_url, STATIC_STRING_SIZE-1-strlen(strRequest));
	memcpy(&strRequest[strlen(strRequest)], " FSP/1.0\r\nHostname: ", STATIC_STRING_SIZE-1-strlen(strRequest));
	memcpy(&strRequest[strlen(strRequest)], file_server_name, STATIC_STRING_SIZE-1-strlen(strRequest));
	memcpy(&strRequest[strlen(strRequest)], "\r\nAgent: xjirgl01\r\n\r\n", STATIC_STRING_SIZE-1-strlen(strRequest));
	char strResponse[STATIC_STRING_SIZE] = {0};
	
	
	// request headers
	printf("\n------------------------------ DOWNLOAD FILE ------------------------------\n");
	printf("%s\n",file_url);
	printf("------------------------------\n");
	printf("DOWNLOAD FILE - REQUEST HEADERS\n");
	printf("------------------------------\n");
	printf("REQUEST: \n%s\n", strRequest);
	printf("----------\n");
	
	int socket = 0;
	if(0 >= (socket = TCPrequestHeaders(file_server_ip, file_server_port, strRequest, strResponse, STATIC_STRING_SIZE-1))){
		exit(EXIT_FAILURE);
	}
	
	printf("----------\n");
	printf("RESPONSE: \n%s\n", strResponse);
	
	
	// check server response
	printf("\n------------------------------\n");
	printf("DOWNLOAD FILE - CHECK RESPONSE\n");
	printf("------------------------------\n");
	
	if(strstr(strResponse, "Success") == NULL){
		char response[STATIC_STRING_SIZE] = {0};
		recv(socket, response, sizeof(response)-1, 0);
		close(socket);
		fprintf(stderr,"ERROR - FILE NOT SAVED: %s\n", response);
		exit(EXIT_FAILURE);
	} else {
		printf("OK\n");
	}
	
	
	// create directories in the file path
	printf("\n------------------------------\n");
	printf("DOWNLOAD FILE - CREATE DIRS\n");
	printf("------------------------------\n");
	printf("%s\n",file_url);
	printf("----------\n");
	
	char dirs[STATIC_STRING_SIZE] = {0};
	memcpy(dirs, file_url, STATIC_STRING_SIZE-1);
	int dirs_count = 0;
	char * last_pos = dirs;
	char * current_pos = NULL;
	while((current_pos = strstr(dirs,"/")) != NULL){
		current_pos[0] = ' ';
		
		// get dir name
		char dirName[STATIC_STRING_SIZE] = {0};
		memcpy(dirName, last_pos, current_pos-last_pos);
		printf("DIR: %s\n",dirName);
		last_pos = current_pos+1;
		
		// create dir
		if(mkdir(dirName, S_IRWXU) == -1) {
			if(errno != EEXIST) {
				close(socket);
				fprintf(stderr,"INTERNAL ERROR - CAN'T CREATE DIR: %d\n", errno);
				exit(EXIT_FAILURE);
			}
		}
		chdir(dirName);
		
		dirs_count++;
	}
	
	printf("DIRS COUNT: %d\n",dirs_count);
	
	
	// download file
	printf("\n------------------------------\n");
	printf("DOWNLOAD FILE - SAVE FILE\n");
	printf("------------------------------\n");
	
	if(0 != TCPsocketSaveFile(socket,SOCKET_FILE_BUFFER_SIZE,last_pos)){
		fprintf(stderr,"ERROR - FILE NOT SAVED: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	
	printf("------------------------------\n");
	printf("FILE SAVED OK\n");
	
	
	// back to start dir
	while(dirs_count-- > 0){
		chdir("..");
	}
}

/****************************************************** CUSTOM FUNCTIONS ******************************************************************************/

void trim(char * str)
{
	char * front = str-1;
	while(isspace(*++front));
	char * back = front+strlen(front);
	while(isspace(*--back));
	*(++back) = '\0';
	if(front != str) memcpy(str, front, back-front+1);
}


void strtolower(char *str)
{
	while(*str) *(str++) = tolower((unsigned char) *str);
}



/****************************************************** NETWORKING FUNCTIONS ******************************************************************************/

int TCPsocketSaveFile(int client_socket, unsigned int buffer_size, char * filename)
{
	char response[buffer_size];
	// clear buffer
	memset(&response, 0, sizeof(response));
	
	FILE *file = fopen(filename,"wb");
	if(file == NULL){
		fprintf(stderr,"ERROR - TCP socket save file: CREATE FILE: %s\n", filename);
		close(client_socket);
		return -7;
	}
	
	// block timeout
	struct timeval timeoutStruct;
	memset(&timeoutStruct, 0, sizeof(timeoutStruct));
	timeoutStruct.tv_sec = SOCKET_BLOCK_TIMEOUT; // socket block timeout (seconds)
	fd_set readable_socket; // socket file descriptors
	
	// -------------------------
	// SOCKET LOOP
	int bytesSent = 0;
	int bytesReceived = 0;
	int timeout = 30*SOCKET_READ_TIMEOUT; // 30 seconds
	while(timeout-- > 0){
		FD_ZERO(&readable_socket);
		FD_SET(client_socket, &readable_socket);
		
		// check file descriptors
		select(FD_SETSIZE, &readable_socket, NULL, NULL, &timeoutStruct);
		
		// ready to read
		if(FD_ISSET(client_socket, &readable_socket)){
			printf("READ\n");
			int bytesLength = recv(client_socket, response, sizeof(response)-1, 0);
			if(bytesLength > 0){
				// bytes received OK
				bytesReceived += bytesLength;
				
				// write to the file
				if(fwrite(response, 1, bytesLength, file) != bytesLength){
					fprintf(stderr,"ERROR - TCP socket save file: unable to write to the file: %d\n", errno);
					// close client socket
					close(client_socket);
					fclose(file);
					return -8;
				}
				
				// clear buffer
				memset(&response, 0, bytesLength);
				
			} else if(bytesLength == 0){
				// proper shutdown - "end-of-file" received
				printf("DONE - END OF FILE\n");
				break;
			} else if(bytesLength < 0){
				fprintf(stderr,"ERROR - TCP socket save file: recv: %d\n", errno);
				// close client socket
				close(client_socket);
				fclose(file);
				return -4;
			}
			printf("Bytes received: %d\n", bytesLength);
			timeout++;
			
		} else if(bytesReceived != 0 && errno != EAGAIN) {
			// reading done OK
			printf("DONE - NOTHING TO READ\n");
			break;
		}
		
		// short delay
		usleep(1000000/30);
	}
	if(timeout == 0){
		// timeout
		printf("DONE - TIMEOUT\n");
	}
	
	
	
	// -------------------------
	// FREE SOCKET
	// close client socket
	close(client_socket);
	fclose(file);
	
	// timeout or full buffer
	return timeout == 0;
}


int TCPrequestHeaders(char * server_ip, int server_port, char * request, char * response, int response_size)
{
	// -------------------------
	// PREPARE SOCKET
	struct sockaddr_in server_address;
	socklen_t server_address_size;
	int client_socket;
	
	// prepare server address
	server_address_size = sizeof(server_address);
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);
	if(inet_aton(server_ip, &server_address.sin_addr) == 0) {
		fprintf(stderr, "ERROR - TCP request headers: invalid server address!\n");
		return -1;
	}
	
	// create client socket
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0){
		fprintf(stderr,"ERROR - TCP request headers: couldn't create client socket!\n");
		return -2;
	}
	
	// connect socket
	if(connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0){
		fprintf(stderr,"ERROR - TCP request headers: couldn't connect socket to server: %d\n", errno);
		return -6;
	}
	
	// set non-blocking socket
	int flags = fcntl(client_socket, F_GETFL, 0);
	if(0 > fcntl(client_socket, F_SETFL, flags | O_NONBLOCK)){
		fprintf(stderr,"ERROR - TCP request headers: couldn't set client socket to non-blocking socket!\n");
		return -3;
	}
	
	// block timeout
	struct timeval timeoutStruct;
	memset(&timeoutStruct, 0, sizeof(timeoutStruct));
	timeoutStruct.tv_sec = SOCKET_BLOCK_TIMEOUT; // socket block timeout (seconds)
	fd_set readable_socket, writable_socket; // socket file descriptors
	
	//printf("INFO: client socket: %s:%d \n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
	
	
	
	// -------------------------
	// SOCKET LOOP
	int bytesSent = 0;
	int bytesReceived = 0;
	int timeout = 30*SOCKET_READ_TIMEOUT; // 30 seconds
	while(timeout-- > 0){
		FD_ZERO(&readable_socket);
		FD_ZERO(&writable_socket);
		FD_SET(client_socket, &readable_socket);
		FD_SET(client_socket, &writable_socket);
		
		// check file descriptors
		select(FD_SETSIZE, &readable_socket, &writable_socket, NULL, &timeoutStruct);
		
		// ready to read
		if(FD_ISSET(client_socket, &readable_socket)){
			//printf("READ\n");
			// peek data
			int bytesLength = recv(client_socket, &response[bytesReceived], response_size-bytesReceived, MSG_PEEK); // read only two chars
			if(bytesLength > 0){
				// check empty line
				char * emptyLineFound = strstr(response, "\r\n\r\n");
				if(emptyLineFound != NULL){
					// FOUND - read until empty line
					memset(&response[bytesReceived], 0, bytesLength); // clear buffer
					bytesLength = recv(client_socket, &response[bytesReceived], emptyLineFound-response+4, 0);
				} else {
					// NOT FOUND - read all
					memset(&response[bytesReceived], 0, bytesLength); // clear buffer
					bytesLength = recv(client_socket, &response[bytesReceived], response_size-bytesReceived, 0);
				}
				
				if(bytesLength > 0){
					// bytes received OK
					bytesReceived += bytesLength;
				} else {
					fprintf(stderr,"ERROR - TCP request headers: recv2: %d\n", errno);
					// close client socket
					close(client_socket);
					return -4;
				}
				// finish if headers found
				if(emptyLineFound != NULL){
					printf("TCP REQUEST HEADERS - EMPTY LINE FOUND!\nDONE\n");
					break;
				}
			} else if(bytesLength == 0){
				// proper shutdown - "end-of-file" received
				// or full buffer
				if(bytesReceived == response_size){
					printf("DONE - RESPONSE BUFFER IS FULL\n");
				} else {
					printf("DONE - END OF FILE\n");
				}
				break;
			} else if(bytesLength < 0){
				fprintf(stderr,"ERROR - TCP request headers: recv: %d\n", errno);
				// close client socket
				close(client_socket);
				return -4;
			}
			//printf("Bytes received: %d\n", bytesLength);
			timeout++;
			
		} else if(bytesReceived != 0 && errno != EAGAIN) {
			// reading done OK
			printf("DONE - NOTHING TO READ\n");
			break;
		}
		// ready to write
		if(FD_ISSET(client_socket, &writable_socket)){
			if(bytesSent != strlen(request)){
				//printf("WRITE\n");
				//printf("Sending: %s\n", &request[bytesSent]);
				int bytesLength = send(client_socket, &request[bytesSent], strlen(request)-bytesSent, 0);
				if(bytesLength > 0){
					// bytes sent OK
					bytesSent += bytesLength;
				} else if(bytesLength < 0){
					fprintf(stderr,"ERROR - TCP request headers: send: %d\n", errno);
					// close client socket
					close(client_socket);
					return -5;
				}
				//printf("Bytes sent: %d\n", bytesLength);
				timeout++;
			}
		}
		
		// short delay
		usleep(1000000/30);
	}
	if(timeout == 0){
		// timeout
		printf("DONE - TIMEOUT\n");
		close(client_socket);
		return -9;
	}
	
	
	
	// -------------------------
	// FREE SOCKET
	// close client socket
	//close(client_socket);
	
	return client_socket;
}


int TCPrequest(char * server_ip, int server_port, char * request, char * response, int response_size)
{
	// -------------------------
	// PREPARE SOCKET
	struct sockaddr_in server_address;
	int client_socket;
	
	// prepare destination address
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);
	if(inet_aton(server_ip, &server_address.sin_addr) == 0) {
		fprintf(stderr, "ERROR - TCP request: invalid destination address!\n");
		return -1;
	}
	
	// create client socket
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0){
		fprintf(stderr,"ERROR - TCP request: couldn't create client socket!\n");
		return -2;
	}
	
	// connect socket
	if(connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0){
		fprintf(stderr,"ERROR - TCP request: couldn't connect socket to server: %d\n", errno);
		return -6;
	}
	
	// set non-blocking socket
	int flags = fcntl(client_socket, F_GETFL, 0);
	if(0 > fcntl(client_socket, F_SETFL, flags | O_NONBLOCK)){
		fprintf(stderr,"ERROR - TCP request: couldn't set client socket to non-blocking socket!\n");
		return -3;
	}
	
	// block timeout
	struct timeval timeoutStruct;
	memset(&timeoutStruct, 0, sizeof(timeoutStruct));
	timeoutStruct.tv_sec = SOCKET_BLOCK_TIMEOUT; // socket block timeout (seconds)
	fd_set readable_socket, writable_socket; // socket file descriptors
	
	//printf("INFO: client socket: %s:%d \n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
	
	
	
	// -------------------------
	// SOCKET LOOP
	int bytesSent = 0;
	int bytesReceived = 0;
	int timeout = 30*SOCKET_READ_TIMEOUT; // 30 seconds
	while(timeout-- > 0){
		FD_ZERO(&readable_socket);
		FD_ZERO(&writable_socket);
		FD_SET(client_socket, &readable_socket);
		FD_SET(client_socket, &writable_socket);
		
		// check file descriptors
		select(FD_SETSIZE, &readable_socket, &writable_socket, NULL, &timeoutStruct);
		
		// ready to read
		if(FD_ISSET(client_socket, &readable_socket)){
			//printf("READ\n");
			int bytesLength = recv(client_socket, &response[bytesReceived], response_size-bytesReceived, 0);
			if(bytesLength > 0){
				// bytes received OK
				bytesReceived += bytesLength;
			} else if(bytesLength == 0){
				// proper shutdown - "end-of-file" received
				// or full buffer
				if(bytesReceived == response_size){
					printf("DONE - RESPONSE BUFFER IS FULL\n");
				} else {
					printf("DONE - END OF FILE\n");
				}
				break;
			} else if(bytesLength < 0){
				fprintf(stderr,"ERROR - TCP request: recv: %d\n", errno);
				// close client socket
				close(client_socket);
				return -4;
			}
			//printf("Bytes received: %d\n", bytesLength);
			timeout++;
			
		} else if(bytesReceived != 0 && errno != EAGAIN) {
			// reading done OK
			printf("DONE - NOTHING TO READ\n");
			break;
		}
		// ready to write
		if(FD_ISSET(client_socket, &writable_socket)){
			if(bytesSent != strlen(request)){
				//printf("WRITE\n");
				//printf("Sending: %s\n", &request[bytesSent]);
				int bytesLength = send(client_socket, &request[bytesSent], strlen(request)-bytesSent, 0);
				if(bytesLength > 0){
					// bytes sent OK
					bytesSent += bytesLength;
				} else if(bytesLength < 0){
					fprintf(stderr,"ERROR - TCP request: send: %d\n", errno);
					// close client socket
					close(client_socket);
					return -5;
				}
				//printf("Bytes sent: %d\n", bytesLength);
				timeout++;
			}
		}
		
		// short delay
		usleep(1000000/30);
	}
	if(timeout == 0){
		// timeout
		printf("DONE - TIMEOUT\n");
	}
	
	
	
	// -------------------------
	// FREE SOCKET
	// close client socket
	close(client_socket);
	
	// timeout or full buffer
	return timeout == 0 + (bytesReceived == response_size)*2;
}


int UDPrequest(char * server_ip, int server_port, char * request, char * response, int response_size)
{
	// -------------------------
	// CREATE SOCKET
	struct sockaddr_in server_address;
	socklen_t server_address_size;
	int client_socket;
	
	// prepare destination address
	server_address_size = sizeof(server_address);
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);
	if(inet_aton(server_ip, &server_address.sin_addr) == 0) {
		fprintf(stderr, "ERROR - UDP request: invalid destination address!\n");
		return -1;
	}
	
	// create client socket
	if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0){
		fprintf(stderr,"ERROR - UDP request: couldn't create client socket!\n");
		return -2;
	}
	
	// set non-blocking socket
	int flags = fcntl(client_socket, F_GETFL, 0);
	if(0 > fcntl(client_socket, F_SETFL, flags | O_NONBLOCK)){
		fprintf(stderr,"ERROR - UDP request: couldn't set client socket to non-blocking socket!\n");
		return -3;
	}
	
	// block timeout
	struct timeval timeoutStruct;
	memset(&timeoutStruct, 0, sizeof(timeoutStruct));
	timeoutStruct.tv_sec = SOCKET_BLOCK_TIMEOUT; // socket block timeout (seconds)
	fd_set readable_socket, writable_socket; // socket file descriptors
	
	//printf("INFO: client socket: %s:%d \n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
	
	
	
	// -------------------------
	// SOCKET LOOP
	int bytesSent = 0;
	int bytesReceived = 0;
	int timeout = 30*SOCKET_READ_TIMEOUT; // 30 seconds
	while(timeout-- > 0){
		FD_ZERO(&readable_socket);
		FD_ZERO(&writable_socket);
		FD_SET(client_socket, &readable_socket);
		FD_SET(client_socket, &writable_socket);
		
		// check file descriptors
		select(FD_SETSIZE, &readable_socket, &writable_socket, NULL, &timeoutStruct);
		
		// ready to read
		if(FD_ISSET(client_socket, &readable_socket)){
			printf("READ\n");
			int bytesLength = recvfrom(client_socket, &response[bytesReceived], response_size-bytesReceived, 0, (struct sockaddr *) &server_address, &server_address_size);
			if(bytesLength > 0){
				// bytes received OK
				bytesReceived += bytesLength;
			} else if(bytesLength == 0){
				// proper shutdown - "end-of-file" received
				// or full buffer
				if(bytesReceived == response_size){
					printf("DONE - RESPONSE BUFFER IS FULL\n");
				} else {
					printf("DONE - END OF FILE\n");
				}
				break;
			} else if(bytesLength < 0){
				fprintf(stderr,"ERROR - UDP request: recvfrom: %d\n", errno);
				// close client socket
				close(client_socket);
				return -4;
			}
			printf("Bytes received: %d\n", bytesLength);
			timeout++;
			
		} else if(bytesReceived != 0 && errno != EAGAIN) {
			// reading done OK
			printf("DONE - NOTHING TO READ\n");
			break;
		}
		// ready to write
		if(FD_ISSET(client_socket, &writable_socket)){
			if(bytesSent != strlen(request)){
				//printf("WRITE\n");
				//printf("Sending: %s\n", &request[bytesSent]);
				int bytesLength = sendto(client_socket, &request[bytesSent], strlen(request)-bytesSent, 0, (struct sockaddr *) &server_address, (socklen_t) server_address_size);
				if(bytesLength > 0){
					// bytes sent OK
					bytesSent += bytesLength;
				} else if(bytesLength < 0){
					fprintf(stderr,"ERROR - UDP request: sendto: %d\n", errno);
					// close client socket
					close(client_socket);
					return -5;
				}
				//printf("Bytes sent: %d\n", bytesLength);
				timeout++;
			}
		}
		
		// short delay
		usleep(1000000/30);
	}
	if(timeout == 0){
		// timeout
		printf("DONE - TIMEOUT\n");
	}
	
	
	
	// -------------------------
	// FREE SOCKET
	// close client socket
	close(client_socket);
	
	// timeout or full buffer
	return timeout == 0 + (bytesReceived == response_size)*2;
}
