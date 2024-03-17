#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>
#define RCVBUFSIZE 100000 //big enough to fit TMDG.html

void die_with_error(char *errorMessage);

int main(int argc, char *argv[]) {
    int servSock;                   // Socket descriptor for server
    int clntSock;                   // Socket descriptor for client
    struct sockaddr_in servAddr;    // Local address
    struct sockaddr_in clntAddr;    // Client address
    unsigned short servPort;        // Server port
    unsigned int clntLen;           // Length of client address data structure
    char http_request[RCVBUFSIZE];  // Buffer for HTTP request
    char http_response[RCVBUFSIZE]; // Buffer for HTTP response
    int bytes_received;             // Number of bytes received from client

    // Check for correct number of arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]);
        exit(1);
    }

    servPort = atoi(argv[1]); // First argument: local port

    // Create socket for incoming connections
    if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        die_with_error("socket() failed");
    }

    // Construct local address structure
    memset(&servAddr, 0, sizeof(servAddr));         // Zero out structure
    servAddr.sin_family = AF_INET;                  // Internet address family
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);   // Any incoming interface
    servAddr.sin_port = htons(servPort);            // Local port

    // Bind to the local address
    if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        die_with_error("bind() failed");
    }

    // Mark the socket so it will listen for incoming connections
    if (listen(servSock, 5) < 0) {
        die_with_error("listen() failed");
    }

    printf("Waiting for connections on port %d...\n", servPort);

    // Loop forever to accept incoming connections
    while (1) {
        // Set the size of the in-out parameter
        clntLen = sizeof(clntAddr);

        // Wait for a client to connect
        if ((clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntLen)) < 0) {
            die_with_error("accept() failed");
        }

        printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));

        // Receive HTTP request from client
        bytes_received = recv(clntSock, http_request, RCVBUFSIZE - 1, 0);
        if (bytes_received < 0) {
            die_with_error("recv() failed");
        }

        // Null-terminate the string received from client
        http_request[bytes_received] = '\0';

        printf("Received HTTP request from client: %s\n", http_request);
        

// is the request looking for a file?
char *filepath;
char *slash_pos = strchr(http_request, '/'); // save file path starting at first /

FILE *file_ptr;
long file_size;

//if (slash_pos != NULL) { // there is a filepath
char *end_pos = strchr(slash_pos, ' '); // find the end position of the filepath
int filepath_len = end_pos - slash_pos - 1; // calculate the length of the filepath
if(filepath_len > 1){ //actual filepath detected and not just a /
filepath = calloc(filepath_len + 1, sizeof(char)); // allocate memory for filepath
strncpy(filepath, slash_pos + 1, filepath_len); // copy the filepath from http_request
filepath[filepath_len] = '\0'; // terminate the filepath string
printf("Filepath: %s\n", filepath);
file_ptr = fopen(filepath, "r"); // open the found filepath
if (file_ptr == NULL) {
    printf("Unable to open file\n");
    exit(1);
}
fseek(file_ptr, 0, SEEK_END); // get the file size
file_size = ftell(file_ptr);
rewind(file_ptr);
// read the contents into http_response
if (file_size >= RCVBUFSIZE) {
    printf("File size is too large to fit in buffer\n");
    exit(1);
} else {
    fread(http_response, sizeof(char), file_size, file_ptr);
    http_response[file_size] = '\0';
}
fclose(file_ptr);
free(filepath); // free the allocated memory for filepath
} 
else {
printf("No filepath found.\n");
sprintf(http_response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
}

// Send HTTP response back to client
if (send(clntSock, http_response, strlen(http_response), 0) < 0) {
die_with_error("send() failed");
}

// Close client socket
close(clntSock);
    }

}

void die_with_error(char *errorMessage) {
    fprintf(stderr, "%s\n", errorMessage);
    exit(1);
}
