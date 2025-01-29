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
    int socket_fd;
    struct sockaddr_in server_addr;
    char message[1000];
    char server_reply[1000];
    
    // Create socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9002);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    // Connect to server
    connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("Connected to server\n");
    
    while(1) {
        printf("Enter message: ");
        fgets(message, 1000, stdin);
        
        if(strncmp(message, "quit", 4) == 0) {
            send(socket_fd, message, strlen(message), 0);
            break;
        }
        
        // Send message to server
        send(socket_fd, message, strlen(message), 0);
        
        // Receive server's reply
        recv(socket_fd, server_reply, 1000, 0);
        printf("Server sent: %s", server_reply);
    }
    
    close(socket_fd);
    return 0;
}