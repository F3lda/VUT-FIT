/**
 * @file server.c (IPK projekt 2)
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

#include <ifaddrs.h>
#include <linux/if_link.h>

#include <sys/ioctl.h>
#include <net/if.h>

#include <time.h>
#include <dirent.h> 


#include "../libs/processes.h"


#define MACHINE_NAME "IPK-SFTP-SERVER"


#define STDIN 0

#define STATIC_STRING_SIZE 512
#define MESSAGE_NULL_CHAR 1


struct client_info {
    int socket;
    char ip_address[STATIC_STRING_SIZE];
    int port;
    char pass_file[STATIC_STRING_SIZE];
};


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

int client_process(pid_t processID, void *userData);

int lookup_host(const char *host, struct address_info_INET64 *addr_info);

int get_interfaces(char * interface_name, char * interface_ip, int max_interface_ip_length, char * list, int max_list_length);

void trim(char * str);

int get_user_line(char * filename, char * user_line, int user_line_size);


int main(int argc, char *argv[])
{
    // READ ARGS
    // -------------------------
    char arg_interface[STATIC_STRING_SIZE] = {0};
    int arg_port = 0;
    char arg_pass_file_path[STATIC_STRING_SIZE] = {0};
    
    char usage_str[] = "usage: %s {-i <interface>} {-p <port>} -u <passwords file>\n";
    
    int arg_check_sum = 0; // for mandatory arguments only
    int i = 0;
    while(++i < argc)
    {
        char *value1 = i+1 < argc ? argv[i+1] : NULL;
        if (strlen(argv[i]) == 2 && argv[i][0] == '-') {
            int option = argv[i][1];
            
            switch(option)
            {
                case 'i':
                case 'p':
                case 'u':
                {
                    if (value1 == NULL) {
                        
                        fprintf(stderr,"ERROR - arguments: option %c - mmissing value\n%s", option, usage_str);
                        exit(EXIT_FAILURE);
                        
                    } else if (option == 'i') {
                        
                        if (arg_interface[0] == '\0') {
                            
                            if (strlen(value1) < sizeof(arg_interface)) {
                            
                                memcpy(arg_interface, value1, sizeof(arg_interface));
                            
                            } else {
                                
                                fprintf(stderr,"ERROR - arguments: option %c - value is too long (max size: %d chars)\n%s", sizeof(arg_interface)-1, option, usage_str);
                                exit(EXIT_FAILURE);
                                
                            }
                            
                        } else {
                            
                            fprintf(stderr,"ERROR - arguments: option %c - duplicate\n%s", option, usage_str);
                            exit(EXIT_FAILURE);
                            
                        }
                        
                    } else if (option == 'p') {
                        
                        if (arg_port == 0) {
                            
                            arg_port = atoi(value1);
                            if (arg_port == 0) {
                                fprintf(stderr,"ERROR - arguments: option %c - invalid value\n%s", option, usage_str);
                                exit(EXIT_FAILURE);
                            }
                            
                        } else {
                            
                            fprintf(stderr,"ERROR - arguments: option %c - duplicate\n%s", option, usage_str);
                            exit(EXIT_FAILURE);
                            
                        }
                        
                    } else if (option == 'u') {
                        
                        if (arg_pass_file_path[0] == '\0') {
                            
                            if (strlen(value1) < sizeof(arg_pass_file_path)) {
                            
                                memcpy(arg_pass_file_path, value1, sizeof(arg_pass_file_path));
                            
                            } else {
                                
                                fprintf(stderr,"ERROR - arguments: option %c - value is too long (max size: %d chars)\n%s", sizeof(arg_pass_file_path)-1, option, usage_str);
                                exit(EXIT_FAILURE);
                                
                            }
                            
                        } else {
                            
                            fprintf(stderr,"ERROR - arguments: option %c - duplicate\n%s", option, usage_str);
                            exit(EXIT_FAILURE);
                            
                        }
                        
                        arg_check_sum++; // mandatory argument
                    }
                    break;
                }
                default:
                {
                    fprintf(stderr,"ERROR - arguments: option %c - unknown\n%s", option, usage_str);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    if (arg_check_sum != 1) {// number of mandatory arguments
        fprintf(stderr,"ERROR - arguments: missing arguments\n%s", usage_str);
        exit(EXIT_FAILURE);
    }
    if (arg_port == 0) {arg_port = 115;}
    
    
    
    
    
    // CHECK ARGS
    // -------------------------
	// check file
    FILE *file;
    if ((file = fopen(arg_pass_file_path, "r")) == NULL) {
        perror("ERROR - password file doesn't exist");
        exit(EXIT_FAILURE);
    }
    fclose(file);
    
    // check interface
    char interface_ip[STATIC_STRING_SIZE] = {0};
    char interface_list[1024] = {0};
    get_interfaces(arg_interface, interface_ip, STATIC_STRING_SIZE-1, interface_list, 1023);
    
    printf("%s\n", interface_list);
    
    if (arg_interface[0] != '\0' && interface_ip[0] == '\0') {
        perror("ERROR - interface ip not found");
        exit(EXIT_FAILURE);
    } else if (interface_ip[0] == '\0') {
        strcpy(interface_ip, "::"); // = INADDR6_ANY -> eqivalent for 0.0.0.0
        // AF_INET6 -> for both IPv4 and IPv6 (for only IPv6 -> socket option - IPV6_V6ONLY) -> https://www.ibm.com/docs/en/i/7.4?topic=sscaaiic-example-accepting-connections-from-both-ipv6-ipv4-clients
    }
    
    printf("SELECTED INTERFACE IP: %s (:: = all interfaces)\n\n", interface_ip);
    printf("----------------------------\n\n");
    
    
    
    
    
    // CREATE SERVER
    // -------------------------
    
    // prepare socket
    int welcome_socket;
    struct address_info_INET64 addr_info;
    memset(&addr_info, 0, sizeof(addr_info));
    
    // set local ip and family
    lookup_host(interface_ip, &addr_info);
    
    // set local port
    if (addr_info.family == AF_INET) {
        
        //printf("IPv4\n");
        //addr_info.addr.v4.sin_family = AF_INET;
        addr_info.addr.v4.sin_port = htons(arg_port);
        // bind socket to specific interface (selects the first interface with the name equal to the name inserted by user -> maybe TODO option '-6' -> for the first ipv6 interface)
        //inet_pton(AF_INET, addr_info.address, (void *)&addr_info.addr.v4.sin_addr.s_addr);
        
    } else if (addr_info.family == AF_INET6) { // PF_INET6 -> https://stackoverflow.com/questions/6729366/what-is-the-difference-between-af-inet-and-pf-inet-in-socket-programming
        
        //printf("IPv6\n");
        //addr_info.addr.v6.sin6_family = AF_INET6;
        addr_info.addr.v6.sin6_port = htons(arg_port);
        // bind socket to specific interface (selects the first interface with the name equal to the name inserted by user -> maybe TODO option '-6' -> for the first ipv6 interface)
        //inet_pton(AF_INET6, addr_info.address, (void *)&addr_info.addr.v6.sin6_addr.s6_addr);
        //addr_info.addr.v6.sin6_scope_id = if_nametoindex(arg_interface);
        
    }
    
    
    
    // create server main socket
	if ((welcome_socket = socket(addr_info.family, SOCK_STREAM, 0)) < 0) {
		perror("ERROR - socket");
		exit(EXIT_FAILURE);
	}

    // enable immediate address reuse
    int enable = 1;
    if (setsockopt(welcome_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
		perror("ERROR - SO_REUSEADDR");
		exit(EXIT_FAILURE);
    }
    
    // bind socket to the port
    if (bind(welcome_socket, (struct sockaddr*)&addr_info.addr, sizeof(addr_info.addr)) < 0) {
		
        perror("ERROR - bind port");
		exit(EXIT_FAILURE);
	}
    
	if ((listen(welcome_socket, 16)) < 0) {
		
        perror("ERROR - listen");
		exit(EXIT_FAILURE);
	}
    
    // set non-blocking socket
	int flags = fcntl(welcome_socket, F_GETFL, 0);
	if (fcntl(welcome_socket, F_SETFL, flags | O_NONBLOCK) < 0) {
		perror("ERROR - non-blocking");
		exit(EXIT_FAILURE);
	}
	
	// socket blocking timeout
	struct timeval timeoutStruct = {0, 0}; // socket blocking timeout (seconds, nanoseconds)
	fd_set fd_welcome_socket; // socket file descriptors
    
    
    
    
    
    // SERVER SOCKET LOOP
	// -------------------------
    
    printf("\n\nSERVER STARTED SUCCESSFULLY!\n");
    printf("--------------------------------------------------------\n");
    
	struct sockaddr_in6 sa_client;
    socklen_t sa_client_len = sizeof(sa_client);
    
	char ip_buffer[INET6_ADDRSTRLEN];
    
    struct client_info clinfo;
    memset(&clinfo, 0, sizeof(clinfo));
    
    fd_set fds_stdin;
    
    while(1)
	{
        // check a new client
        FD_ZERO(&fd_welcome_socket);
        FD_SET(welcome_socket, &fd_welcome_socket);
        if (select(FD_SETSIZE, &fd_welcome_socket, NULL, NULL, &timeoutStruct) == 1) {
            
            // accept new client
            int new_client_socket = accept(welcome_socket, (struct sockaddr*)&sa_client, &sa_client_len);		
             
            if (new_client_socket >= 0) {
                
                // create info struct for the new client and process
                memset(&clinfo, 0, sizeof(clinfo));
                clinfo.socket = new_client_socket;
                
                if(inet_ntop(AF_INET6, &sa_client.sin6_addr, ip_buffer, sizeof(ip_buffer))) {
                
                    memcpy(clinfo.ip_address, ip_buffer, sizeof(clinfo.ip_address));
                    clinfo.port = ntohs(sa_client.sin6_port);
                
                }
                
                memcpy(clinfo.pass_file, arg_pass_file_path, sizeof(clinfo.pass_file));
                
                // create the new process for the new client
                ProcessCreate(client_process, &clinfo, false);
                
            } else {
                // error
                break;
            }
        }
        
        
        
        // check an input on STDIN
        FD_ZERO(&fds_stdin);
        FD_SET(STDIN, &fds_stdin);
        if (select(1, &fds_stdin, NULL, NULL, &timeoutStruct) > 0) {
            
            // read line from STDIN
            char stdin_str[STATIC_STRING_SIZE] = {0};
            fgets(stdin_str, sizeof(stdin_str), stdin);
            trim(stdin_str);
            if (strcmp(stdin_str, "DONE") == 0) {
                
                while(ProcessCheckIfAnyChildProcessFinished() > 0){} // read all finished processes
                
                // check if any clients are still connected
                if (ProcessCheckIfAnyChildProcessFinished() == 0) {
                    printf("SOME CLIENTS STILL CONNECTED!\nKILLING ALL PROCESSES...\n");
                    close(welcome_socket);
                    kill(0, SIGKILL);
                }
                
                errno = 0;
                break;
            }
            // write line to STDOUT
            if (stdin_str[0] != '\0') {
                printf("> %s\n", stdin_str);
            }
        }
        
        
        
        // short delay
		usleep(1000000/30);
	}
    
    
    
	// -------------------------
	// FREE SOCKET
	// close server socket
	close(welcome_socket);

    printf("SERVER FINISHED OK: %s\n", strerror(errno));

    return 0;
}


int client_process(pid_t processID, void *userData)
{
    // read client info struct
    struct client_info clinfo;
    memset(&clinfo, 0, sizeof(clinfo));
    clinfo = *((struct client_info *)userData);
    
    
    printf("NEW CLIENT CONNECTED: [%s]:%d\n", clinfo.ip_address, clinfo.port);
    printf("PASS FILE: %s\n", clinfo.pass_file);
    
    int client_socket = clinfo.socket;
    
    // send welcome message
    send(client_socket, "+", 1, 0);
    send(client_socket, MACHINE_NAME, strlen(MACHINE_NAME), 0);
    send(client_socket, " SFTP Service", strlen(" SFTP Service")+MESSAGE_NULL_CHAR, 0);
    
    // vars
    int LOGGED_IN = 0;
    char LAST_CMD[5] = {0};
    char user_line[STATIC_STRING_SIZE] = {0};
    char STREAM_TYPE = 'B';
    int DIR_COUNT = 0;
    char RENAME_FILE[STATIC_STRING_SIZE] = {0};
    char DOWNLOAD_FILE[STATIC_STRING_SIZE] = {0};
    
    char message_buffer[STATIC_STRING_SIZE] = {0};
    int receivedBytes = 0;
    while(1)		
    {
        receivedBytes = recv(client_socket, message_buffer, sizeof(message_buffer),0);
        
        if (receivedBytes <= 0)                
            break;
        
        printf("MESSAGE ([%s]:%d): <%s>(%d)\n", clinfo.ip_address, clinfo.port, message_buffer, receivedBytes);
        
        
        
        // handle message and send response
        //-------------------------------------------------
        
        // check if file bytes are not in MESSAGE
        if (strcmp(LAST_CMD, "SIZE") != 0) { // strcmp(LAST_CMD, "SEND") != 0 && 
        
            // handle current CMD
            
            // DONE
            if (strncmp(message_buffer, "DONE", 4) == 0) {
                
                memcpy(message_buffer, "+DONE - close the connection", strlen("+DONE - close the connection")+1);
            
            // USER id - ACCT account
            } else if (strncmp(message_buffer, "USER", 4) == 0 || strncmp(message_buffer, "ACCT", 4) == 0) {
                
                // username received
                if (receivedBytes > 6) {
                    char *username = message_buffer+5;
                    printf("USERNAME: %s\n", username);
                    
                    // already logged in
                    if (LOGGED_IN == 1 && strstr(user_line, username) == user_line) {  
                        
                        if (strncmp(message_buffer, "USER", 4) == 0) {// USER
                            char str_temp[STATIC_STRING_SIZE] = {0};
                            snprintf(str_temp, sizeof(str_temp), "!%s logged in", username);
                            memcpy(message_buffer, str_temp, strlen(str_temp)+1);
                        } else {// ACCT
                            memcpy(message_buffer, "! Account valid, logged-in", strlen("! Account valid, logged-in")+1);
                        }
                    
                    // new user
                    } else {
                        LOGGED_IN = 0;
                        
                        memset(user_line, '\0', sizeof(user_line));
                        memcpy(user_line, username, sizeof(user_line));
                        int userNeeded = get_user_line(clinfo.pass_file, user_line, sizeof(user_line));
                        
                        // invalid user id
                        if (userNeeded == 1 && user_line[0] == '\0') {
                            if (strncmp(message_buffer, "USER", 4) == 0) {// USER
                                memcpy(message_buffer, "-Invalid user-id, try again", strlen("-Invalid user-id, try again")+1);
                            } else {// ACCT
                                memcpy(message_buffer, "-Invalid account, try again", strlen("-Invalid account, try again")+1);
                            }
                        
                        // valid user id - need password
                        } else if (userNeeded == 1 && user_line[0] != '\0' && (strchr(user_line, ':')+1)[0] != '\0') {
                            if (strncmp(message_buffer, "USER", 4) == 0) {// USER
                                memcpy(message_buffer, "+User-id valid, send account and password", strlen("+User-id valid, send account and password")+1);
                            } else {// ACCT
                                memcpy(message_buffer, "+Account valid, send password", strlen("+Account valid, send password")+1);
                            }
                            
                        // user id or password not needed
                        } else {
                            
                            // add user id to user_line
                            snprintf(user_line, sizeof(user_line), "%s:", username);
                            
                            if (strncmp(message_buffer, "USER", 4) == 0) {// USER
                                char str_temp[STATIC_STRING_SIZE] = {0};
                                snprintf(str_temp, sizeof(str_temp), "!%s logged in", username);
                                memcpy(message_buffer, str_temp, strlen(str_temp)+1);
                            } else {// ACCT
                                memcpy(message_buffer, "! Account valid, logged-in", strlen("! Account valid, logged-in")+1);
                            }
                            
                            LOGGED_IN = 1;
                        }
                        
                        printf("USER: needed: %d - found: %s\n", userNeeded, user_line);
                        
                    }
                    
                // no username received
                } else {
                    if (strncmp(message_buffer, "USER", 4) == 0) {// USER
                        memcpy(message_buffer, "-Invalid user-id, try again", strlen("-Invalid user-id, try again")+1);
                    } else {// ACCT
                        memcpy(message_buffer, "-Invalid account, try again", strlen("-Invalid account, try again")+1);
                    }
                }
                
                
            // PASS password
            } else if (strncmp(message_buffer, "PASS", 4) == 0) {
                
                // password received
                if (receivedBytes > 6) {
                    char *password = message_buffer+5;
                    printf("PASSWORD: %s\n", password);
                
                    // no username or already logged in
                    if (user_line[0] == '\0' || LOGGED_IN == 1) {
                        
                        memcpy(message_buffer, "+Send account", strlen("+Send account")+1);
                        
                    // password ok
                    } else if (strcmp((strstr(user_line, ":")+1), password) == 0) {
                        
                        LOGGED_IN = 1;
                        
                        memcpy(message_buffer, "! Logged in", strlen("! Logged in")+1);
                    
                    // invalid password
                    } else {
                        memcpy(message_buffer, "-Wrong password, try again", strlen("-Wrong password, try again")+1);
                    }
                    
                // no password received
                } else {
                    memcpy(message_buffer, "-Wrong password, try again", strlen("-Wrong password, try again")+1);
                }
                
                
            // CMDS AFTER LOGIN
            } else if (LOGGED_IN == 1) {
                
                
                
                
                
                // TYPE {A, B, C}
                if (strncmp(message_buffer, "TYPE", 4) == 0) {
                
                    if (receivedBytes == 7) {
                        
                        // ACII
                        if (message_buffer[5] == 'A') {
                            STREAM_TYPE = message_buffer[5];
                            memcpy(message_buffer, "+Using Ascii mode", strlen("+Using Ascii mode")+1);
                            
                        // BINARY
                        } else if(message_buffer[5] == 'B') {
                            STREAM_TYPE = message_buffer[5];
                            memcpy(message_buffer, "+Using Binary mode", strlen("+Using Binary mode")+1);
                            
                        // CONTINUOUS
                        } else if(message_buffer[5] == 'C') {
                            STREAM_TYPE = message_buffer[5];
                            memcpy(message_buffer, "+Using Continuous mode", strlen("+Using Continuous mode")+1);
                            
                        // invalid type
                        } else {
                            memcpy(message_buffer, "-Type not valid", strlen("-Type not valid")+1);
                        }
                        
                    // invalid type
                    } else {
                        memcpy(message_buffer, "-Type not valid", strlen("-Type not valid")+1);
                    }
                
                
                
                // LIST {F, V} directory-path
                } else if (strncmp(message_buffer, "LIST", 4) == 0 && (receivedBytes == 7 || (receivedBytes > 8 && message_buffer[6] == ' '))) {
                    
                    if (message_buffer[5] == 'F' || message_buffer[5] == 'V') {
                        char curdir[2] = ".";
                        char *path = curdir;
                        if (receivedBytes > 8) {
                            path = message_buffer+7;
                        }
                        
                        if (strstr(path, "..") == NULL) { // path can't go up
                            char tempBuffer[STATIC_STRING_SIZE] = {0};
                            strcpy(tempBuffer, "+");
                            strcat(tempBuffer, path);
                            strcat(tempBuffer, "\r\n");
                            send(client_socket, tempBuffer, strlen(tempBuffer), 0);
                            
                            printf("path: %s\n", path);
                            
                            DIR *directory;
                            struct dirent *dir;
                            directory = opendir(path);
                            if (directory) {
                                
                                while ((dir = readdir(directory)) != NULL) {
                                    if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                                        
                                        if (message_buffer[5] == 'F') { // F format
                                            snprintf(tempBuffer, sizeof(tempBuffer), "%s\r\n", dir->d_name);
                                            
                                        } else { // V format
                                            char filePath[STATIC_STRING_SIZE] = {0};
                                            snprintf(filePath, sizeof(filePath), "%s/%s", path, dir->d_name);
                                        
                                            struct stat sb;
                                            if (stat(filePath, &sb) == -1) {
                                                snprintf(tempBuffer, sizeof(tempBuffer), "%s - no info\r\n", dir->d_name);
                                            } else {
                                                snprintf(tempBuffer, sizeof(tempBuffer), "%s\t\t%lld bytes\tUID=%ld   GID=%ld\t%s\r\n", dir->d_name, (long long) sb.st_size, (long) sb.st_uid, (long) sb.st_gid, ctime(&sb.st_mtime));
                                            }
                                            
                                        }
                                        //printf("%s\n", tempBuffer);
                                        send(client_socket, tempBuffer, strlen(tempBuffer), 0);
                                    }
                                }
                                closedir(directory);
                                
                                memset(message_buffer, 0, sizeof(message_buffer)); // send MESSAGE_NULL_CHAR
                            
                            
                            } else {
                                memcpy(message_buffer, "-List path not valid", strlen("-List path not valid")+1);
                            }
                        
                        } else {
                            memcpy(message_buffer, "-List path can't go up", strlen("-List path can't go up")+1);
                        }
                        
                    // invalid 
                    } else {
                        memcpy(message_buffer, "-List format not valid", strlen("-List format not valid")+1);
                    }
                    
                
                
                // CDIR new-directory
                } else if (strncmp(message_buffer, "CDIR", 4) == 0 && receivedBytes > 6) {
                    
                    char *path = message_buffer+5;
                    printf("DIR: %s\n", path);
                    printf("DIR COUNT: %d\n", DIR_COUNT);
                    
                    if (strcmp(path, "..") != 0 || DIR_COUNT > 0) { // check top dir
                        
                        if (strchr(path, '\\') == NULL && strchr(path, '/') == NULL) { // only one dir
                            
                            DIR* dir = opendir(path);
                            if (dir) {
                                closedir(dir);
                                
                                if (chdir(path) == 0) {
                                    
                                    if (strcmp(path, "..") == 0) { // add/sub dir depth
                                        DIR_COUNT--;
                                    } else {
                                        DIR_COUNT++;
                                    }
                                    
                                    char str_temp[STATIC_STRING_SIZE] = {0};
                                    snprintf(str_temp, sizeof(str_temp), "!Changed working dir to %s", path);
                                    memcpy(message_buffer, str_temp, strlen(str_temp)+1);
                                    
                                } else {
                                    memcpy(message_buffer, "-Can't connect to directory because: unable to open directory", strlen("-Can't connect to directory because: unable to open directory")+1);
                                }
                                
                            } else if (ENOENT == errno) {
                                memcpy(message_buffer, "-Can't connect to directory because: directory does not exist", strlen("-Can't connect to directory because: directory does not exist")+1);
                            } else {
                                memcpy(message_buffer, "-Can't connect to directory because: unable to open directory", strlen("-Can't connect to directory because: unable to open directory")+1);
                            }
                        
                        } else {
                            memcpy(message_buffer, "-Can't connect to directory because: only one directory can be changed", strlen("-Can't connect to directory because: only one directory can be changed")+1);
                        }
                    
                    } else {
                        memcpy(message_buffer, "-Can't connect to directory because: current directory is top directory", strlen("-Can't connect to directory because: current directory is top directory")+1);
                    }
                    
                    
                    
                // KILL file-name
                } else if (strncmp(message_buffer, "KILL", 4) == 0 && receivedBytes > 6) {
                    
                    char *filename = message_buffer+5;
                    printf("FILE: %s\n", filename);
                    
                    if (strstr(filename, "..") == NULL) { // path can't go up
                        
                        if(remove(filename) == 0) { // remove file
                            char str_temp[STATIC_STRING_SIZE] = {0};
                            snprintf(str_temp, sizeof(str_temp), "+%s deleted", filename);
                            memcpy(message_buffer, str_temp, strlen(str_temp)+1);
                        } else {
                            memcpy(message_buffer, "-Not deleted because unable to delete the file", strlen("-Not deleted because unable to delete the file")+1);
                        }
                    
                    } else {
                        memcpy(message_buffer, "-Not deleted because path can't go up", strlen("-Not deleted because path can't go up")+1);
                    }
                    
                    
                    
                // NAME file-name
                } else if (strncmp(message_buffer, "NAME", 4) == 0 && receivedBytes > 6) {
                    
                    char *filename = message_buffer+5;
                    printf("FILE: %s\n", filename);
                    
                    if (strstr(filename, "..") == NULL) { // path can't go up
                        
                        FILE *file;
                        if ((file = fopen(filename, "r")) != NULL) { // file exists?
                            fclose(file);
                            
                            snprintf(RENAME_FILE, sizeof(RENAME_FILE), "%s", filename); // save file name
                            
                            memcpy(message_buffer, "+File exists", strlen("+File exists")+1);
                            
                        } else {
                            char str_temp[STATIC_STRING_SIZE] = {0};
                            snprintf(str_temp, sizeof(str_temp), "-Can't find %s", filename);
                            memcpy(message_buffer, str_temp, strlen(str_temp)+1);
                        }
                    
                    } else {
                        char str_temp[STATIC_STRING_SIZE] = {0};
                        snprintf(str_temp, sizeof(str_temp), "-Can't find %s because path can't go up", filename);
                        memcpy(message_buffer, str_temp, strlen(str_temp)+1);
                    }
                    
                    
                // TOBE new-name
                } else if (strncmp(message_buffer, "TOBE", 4) == 0 && receivedBytes > 6) {
                    
                    char *filename = message_buffer+5;
                    printf("FILE OLD: %s\n", RENAME_FILE);
                    printf("FILE NEW: %s\n", filename);
                    
                    if (strstr(filename, "..") == NULL) { // path can't go up
                        
                        if(rename(RENAME_FILE, filename) == 0) { // rename file
                            char str_temp[STATIC_STRING_SIZE] = {0};
                            snprintf(str_temp, sizeof(str_temp), "+%s renamed to %s", RENAME_FILE, filename);
                            memcpy(message_buffer, str_temp, strlen(str_temp)+1);
                            
                            memset(RENAME_FILE, 0, sizeof(RENAME_FILE));
                        } else {
                            memcpy(message_buffer, "-File wasn't renamed because unable to rename the file", strlen("-File wasn't renamed because unable to rename the file")+1);
                        }
                    
                    } else {
                        memcpy(message_buffer, "-File wasn't renamed because path can't go up", strlen("-File wasn't renamed because path can't go up")+1);
                    }
                    
                    
                // RETR file-name
                } else if (strncmp(message_buffer, "RETR", 4) == 0 && receivedBytes > 6) {
                    
                    char *filename = message_buffer+5;
                    printf("FILE: %s\n", filename);
                    
                    if (strstr(filename, "..") == NULL) { // path can't go up
                        
                        FILE *file;
                        if ((file = fopen(filename, "r")) != NULL) { // file exists?
                            fclose(file);
                            printf("FILE eXISTS\n");
                            struct stat sb;
                            if (stat(filename, &sb) != -1) { // get file size
                                
                                snprintf(DOWNLOAD_FILE, sizeof(DOWNLOAD_FILE), "%s", filename); // save file name
                                
                                char str_temp[STATIC_STRING_SIZE] = {0};
                                snprintf(str_temp, sizeof(str_temp), " %lld", (long long) sb.st_size);
                                memcpy(message_buffer, str_temp, strlen(str_temp)+1);
                                
                            } else {
                                memcpy(message_buffer, "-File doesn't exist", strlen("-File doesn't exist")+1);
                            }
                            
                        } else {
                            memcpy(message_buffer, "-File doesn't exist", strlen("-File doesn't exist")+1);
                        }
                        
                    } else {
                        memcpy(message_buffer, "-File doesn't exist because path can't go up", strlen("-File doesn't exist because path can't go up")+1);
                    }
                    
                    
                // SEND
                } else if (strncmp(message_buffer, "SEND", 4) == 0) {
                    
                    FILE *download_file = fopen(DOWNLOAD_FILE, "r");
                    if(download_file == NULL){
                        printf("File doesn't exist\n");
                        memcpy(message_buffer, "-File doesn't exist", strlen("-File doesn't exist")+1);
                    } else {
                    
                        char buffer[STATIC_STRING_SIZE] = {0};
                        while(fread(buffer, 1, sizeof(buffer)-1, download_file) > 0) {
                            send(client_socket, buffer, strlen(buffer), 0); // send data
                            memset(buffer, 0, sizeof(buffer)); // clear buffer
                        }
                        
                        fclose(download_file);
                        
                        printf("FILE DOWNLOADED\n");
                        memset(message_buffer, 0, sizeof(message_buffer)); // send MESSAGE_NULL_CHAR
                    }
                    
                    
                // STOP
                } else if (strncmp(message_buffer, "STOP", 4) == 0) {
                    
                    memset(DOWNLOAD_FILE, 0, sizeof(DOWNLOAD_FILE)); // clear download file path
                    memcpy(message_buffer, "+ok, RETR aborted", strlen("+ok, RETR aborted")+1);
                    
                    
                    
                // Invalid command
                } else {
                    memcpy(message_buffer, "-Invalid command", strlen("-Invalid command")+1);
                }
                
                
                
                
                
            // NOT LOGGED IN or Invalid command
            } else {
                memcpy(message_buffer, "-NOT Logged in or Invalid command", strlen("-NOT Logged in or Invalid command")+1);
            }
            
            // save current CMD
            strncpy(LAST_CMD, message_buffer, 4);
        
        }
        
        
        
        
        
        
        //-------------------------------------------------
        // handle message and send response - END
        
        
        
        send(client_socket, message_buffer, strlen(message_buffer)+MESSAGE_NULL_CHAR, 0);
        memset(message_buffer, 0, sizeof(message_buffer));
    }
    
    // FREE SOCKET
    close(client_socket);
    printf("CLIENT DISCONNECTED: [%s]:%d\n", clinfo.ip_address, clinfo.port);
    
    return 0;
}

int get_user_line(char * filename, char * user_line, int user_line_size)
{ // returns true/false if user is needed (file is emtpy)
    
    FILE *passfile = fopen(filename, "r");
    if(passfile == NULL){
        fprintf(stderr, "ERROR - OPEN FILE: %s\n", filename);
        return 0;
    }
    
    int user_found = 0;
    int line_count = 0;
    char buffer[STATIC_STRING_SIZE] = {0};
    while(fgets(buffer, STATIC_STRING_SIZE-1, passfile)) {
        if (strchr(buffer, '\n') != NULL) {line_count++;}
        trim(buffer);
        if (buffer[0] != '\0') {
            if (strstr(buffer, user_line) == buffer && (buffer+strlen(user_line))[0] == ':') { // line starts with username and ':'
                user_found = 1;
                memcpy(user_line, buffer, user_line_size); // return line from file with password
                break;
            }
            //printf("<%s>\n", buffer);
        }
    }
    
    fclose(passfile);
    
    if (user_found == 0) {
        memset(user_line, '\0', user_line_size);
    }
    
    return (line_count > 0);
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

int get_interfaces(char * interface_name, char * interface_ip, int max_interface_ip_length, char * interface_list, int max_list_length)
{// https://man7.org/linux/man-pages/man3/getifaddrs.3.html
    struct ifaddrs *ifaddr, *ifa;
    int family = -1, exit_code = 0;
    char host[NI_MAXHOST];
    
    if (getifaddrs(&ifaddr) == -1) {
        perror("ERROR - getifaddrs");
        return -1;
    }
    
    snprintf(interface_list+strlen(interface_list), max_list_length-strlen(interface_list), "\nLIST OF ETHERNET INTERFACES\n");
    snprintf(interface_list+strlen(interface_list), max_list_length-strlen(interface_list), "----------------------------\n");
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        
        if (ifa->ifa_addr != NULL) {
            
            if (family == AF_PACKET && ifa->ifa_addr->sa_family != AF_PACKET) {
                snprintf(interface_list+strlen(interface_list), max_list_length-strlen(interface_list), "----------------------------\n");
            }
            
            family = ifa->ifa_addr->sa_family;
            
            if (family == AF_INET || family == AF_INET6) {
                
                if (getnameinfo(ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) != 0) {
                    perror("ERROR - getnameinfo");
                    exit_code = 0;
                    break;
                }
                
                snprintf(interface_list+strlen(interface_list), max_list_length-strlen(interface_list), "%-8s %s \t(%d) \taddress: <%s>\n", ifa->ifa_name, (family == AF_PACKET) ? "AF_PACKET" : (family == AF_INET) ? "AF_INET" : (family == AF_INET6) ? "AF_INET6" : "???", family, host);
                
                if (strcmp(ifa->ifa_name, interface_name) == 0 && interface_ip[0] == '\0') {
                    memcpy(interface_ip, host, max_interface_ip_length);
                }
                
            } else if (family == AF_PACKET && ifa->ifa_data != NULL) {
                
                snprintf(interface_list+strlen(interface_list), max_list_length-strlen(interface_list), "%-8s\n", ifa->ifa_name);
                
            }
        }
    }
    snprintf(interface_list+strlen(interface_list), max_list_length-strlen(interface_list), "----------------------------\n");
    
    freeifaddrs(ifaddr);
    
    return exit_code;
}
