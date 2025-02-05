#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

void multiply_numbers(char* input, char* result) {
    char command[512];
    snprintf(command, sizeof(command), "echo '%s' | tr ' ' '*' | bc", input);
    
    FILE* fp = popen(command, "r");  // Execute bash command
    if (fp == NULL) {
        strcpy(result, "Error computing multiplication");
        return;
    }
    
    fgets(result, 256, fp);
    pclose(fp);
}

int main() {
    int socket_fd, client_fd;
    struct sockaddr_in socket_address;
    
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(9002);
    socket_address.sin_addr.s_addr = INADDR_ANY;
    
    bind(socket_fd, (struct sockaddr*)&socket_address, sizeof(socket_address));
    
    listen(socket_fd, 5);
    printf("Waiting for client to connect...\n");

    while (1) {
        client_fd = accept(socket_fd, NULL, NULL);
        if (client_fd == -1) {
            perror("Accept failed");
            continue;
        }
        
        printf("Client connected.\n");

        char recvbuffer[256];
        char sendbuffer[256];

        while (1) {
            memset(recvbuffer, 0, sizeof(recvbuffer));
            int bytes_received = recv(client_fd, recvbuffer, sizeof(recvbuffer), 0);
            
            if (bytes_received <= 0) {
                break;
            }

            printf("Received from client: %s\n", recvbuffer);

            // Calculate multiplication
            multiply_numbers(recvbuffer, sendbuffer);
            send(client_fd, sendbuffer, strlen(sendbuffer), 0);
        }

        printf("Client disconnected.\n");
        close(client_fd);
    }

    close(socket_fd);
    return 0;
}
