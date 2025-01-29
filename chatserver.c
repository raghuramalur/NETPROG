#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void reverse(char str[]) {
    int length = strlen(str);
    int i, j;
    char temp;
    
    for(i = 0, j = length-2; i < j; i++, j--) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

int main() {
    int socket_fd, client_fd;
    struct sockaddr_in server_addr;
    char buffer[1000];
    
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
    
    while(1) {
        // Receive message
        memset(buffer, 0, 1000);
        recv(client_fd, buffer, 1000, 0);
        printf("Client sent: %s", buffer);
        
        if(strncmp(buffer, "quit", 4) == 0) {
            printf("Client disconnected\n");
            break;
        }
        
        // Reverse message
        reverse(buffer);
        
        // Send reversed message back
        send(client_fd, buffer, strlen(buffer), 0);
    }
    
    close(client_fd);
    close(socket_fd);
    return 0;
}