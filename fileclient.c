#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int socket_fd;
    struct sockaddr_in server_addr;
    char buffer[1024];
    FILE *fp;
    
    // Create socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9002);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    // Connect to server
    connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("Connected to server\n");
    
    // Open file to send
    fp = fopen("send.txt", "r");
    if(fp == NULL) {
        printf("Error opening file\n");
        return 1;
    }
    
    // Read file and send its contents
    while(fgets(buffer, 1024, fp) != NULL) {
        send(socket_fd, buffer, strlen(buffer), 0);
        printf("Sending: %s", buffer);
    }
    
    // Send end marker
    strcpy(buffer, "END_OF_FILE");
    send(socket_fd, buffer, strlen(buffer), 0);
    
    printf("File sent successfully\n");
    fclose(fp);
    close(socket_fd);
    return 0;
}