#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int socket_fd, client_fd;
    struct sockaddr_in server_addr;
    char buffer[1024];
    FILE *fp;
    
    // Create socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9002);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    // Bind
    bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    
    // Listen
    listen(socket_fd, 5);
    printf("Server listening...\n");
    
    // Accept connection
    client_fd = accept(socket_fd, NULL, NULL);
    printf("Client connected!\n");
    
    // Open file to write
    fp = fopen("received.txt", "w");
    if(fp == NULL) {
        printf("Error creating file\n");
        return 1;
    }
    
    // Receive file contents
    while(1) {
        memset(buffer, 0, 1024);
        recv(client_fd, buffer, 1024, 0);
        
        if(strcmp(buffer, "END_OF_FILE") == 0) {
            break;
        }
        
        printf("Receiving: %s", buffer);
        fprintf(fp, "%s", buffer);
    }
    
    printf("File received successfully\n");
    fclose(fp);
    close(client_fd);
    close(socket_fd);
    return 0;
}