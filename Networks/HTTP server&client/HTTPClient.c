#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h> 

//getaddrinfo() versionS
#define RCVBUFSIZE 100000 //big enough to receive TMDG.html

void die_with_error(char *errorMessage);

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr;
    char server_url[256];
    char *port_number;
    char http_request[RCVBUFSIZE];
    char http_response[RCVBUFSIZE];
    int print_rtt = 0;
    struct timeval start, end;
    long double rtt_milliseconds = 0;
    FILE *response_file;
    char pathToFile[256] = "/";

    
    //correct num of args?
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s [-p] server_url port_number\n", argv[0]);
        exit(1);
    }

     //is the -p option used?
    if (argc == 4 && strcmp(argv[1], "-p") == 0) {
        print_rtt = 1;
        strcpy(server_url, argv[2]); 
        port_number = argv[3];
    } else {
        strcpy(server_url, argv[1]); 
        port_number = argv[2];
    }

    // create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        die_with_error("socket() failed");
    }


    //Check if the url is trying to locate a file, remove the path to file for now
    char *slash_ptr = strchr(server_url, '/');
    if(slash_ptr != NULL){
        strcpy(pathToFile, slash_ptr);
        *slash_ptr = '\0';
    }

    printf("Server URL: %s\n", server_url);
    printf("Path to File: %s\n", pathToFile);

    
    // resolve server address
    struct addrinfo hints = {0}, *result;
    //memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int rc = getaddrinfo(server_url, NULL, &hints, &result);
    if (rc != 0) {
        fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(rc));
        exit(1);
    }

    // configure server address
    memset(&server_addr, 0, sizeof(server_addr)); //zero out structure, sin_zero
    server_addr.sin_family = AF_INET; //sin_family
    server_addr.sin_addr.s_addr = ((struct sockaddr_in *) result->ai_addr)->sin_addr.s_addr; // server IP
    server_addr.sin_port = htons(atoi(port_number)); //sin_port
    

    printf("Read server address is: %d\nRead server family is:%d\nRead server port is:%d", server_addr.sin_addr.s_addr, server_addr.sin_family, server_addr.sin_port);
    printf("\nAttempting to connect to server...\n");

    // connect to server
    if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        die_with_error("connect() failed");
    }

    printf("Connected to server!\n");

     // build the actual HTTP request
    sprintf(http_request, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", pathToFile, server_url);

    // measure RTT start time
    gettimeofday(&start, NULL);

    // send HTTP request to server
    if (send(sock, http_request, strlen(http_request), 0) < 0) {
        die_with_error("send() failed");
    }

    printf("Sent HTTP request to server: %s", http_request);
    
    // read HTTP response from server
int bytes_received = recv(sock, http_response, RCVBUFSIZE - 1, 0);
if (bytes_received < 0) {
die_with_error("recv() failed");
}

http_response[bytes_received] = '\0'; //terminate the string

// measure RTT end time and calculate RTT
gettimeofday(&end, NULL);
rtt_milliseconds = (end.tv_sec - start.tv_sec) * 1000.0;
rtt_milliseconds += (end.tv_usec - start.tv_usec) / 1000.0;

// print HTTP response
printf("Received HTTP response from server: %s\n", http_response);

// print RTT if requested
if (print_rtt) {
printf("RTT: %.10Lf ms\n", rtt_milliseconds);
}

// close socket
close(sock);

// write HTTP response to file
char* filename = "Response";
if (access(filename, F_OK) != -1) {
// file already exists, so delete it
if (remove(filename) != 0) {
die_with_error("remove() failed");
}
}

// create a new file and write the HTTP response to it
FILE* fp = fopen(filename, "w");
if (fp == NULL) {
die_with_error("fopen() failed");
}

if (fputs(http_response, fp) == EOF) {
die_with_error("fputs() failed");
}

fclose(fp);

exit(0);
}

void die_with_error(char *errorMessage) {
fprintf(stderr, "%s\n", errorMessage);
exit(1);
}
