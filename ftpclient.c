// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define FILE_SIZE 4096

void handle_get_file(int socket_fd, char *filename) {
    char file_buffer[FILE_SIZE];
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Error creating file\n");
        return;
    }
    
    int bytes_received;
    while ((bytes_received = recv(socket_fd, file_buffer, FILE_SIZE, 0)) > 0) {
        fwrite(file_buffer, 1, bytes_received, fp);
        if (bytes_received < FILE_SIZE) break;
    }
    
    fclose(fp);
    printf("File downloaded successfully\n");
}

void handle_put_file(int socket_fd, char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Error: File not found\n");
        return;
    }
    
    char file_buffer[FILE_SIZE];
    size_t bytes_read;
    
    while ((bytes_read = fread(file_buffer, 1, FILE_SIZE, fp)) > 0) {
        send(socket_fd, file_buffer, bytes_read, 0);
    }
    
    fclose(fp);
    
    char response[BUFFER_SIZE];
    recv(socket_fd, response, BUFFER_SIZE, 0);
    printf("%s", response);
}

int main() {
    int socket_fd;
    struct sockaddr_in server_addr;
    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    
    // Create socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9002);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    // Connect to server
    connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("Connected to FTP server\n");
    
    while(1) {
        printf("\nAvailable commands:\n");
        printf("LIST - List files\n");
        printf("GET filename - Download file\n");
        printf("PUT filename - Upload file\n");
        printf("QUIT - Exit\n");
        printf("Enter command: ");
        
        fgets(command, BUFFER_SIZE, stdin);
        command[strlen(command)-1] = '\0';  // Remove newline
        
        send(socket_fd, command, strlen(command), 0);
        
        if (strncmp(command, "LIST", 4) == 0) {
            memset(response, 0, BUFFER_SIZE);
            recv(socket_fd, response, BUFFER_SIZE, 0);
            printf("\nFiles on server:\n%s", response);
        }
        else if (strncmp(command, "GET", 3) == 0) {
            handle_get_file(socket_fd, command + 4);
        }
        else if (strncmp(command, "PUT", 3) == 0) {
            handle_put_file(socket_fd, command + 4);
        }
        else if (strncmp(command, "QUIT", 4) == 0) {
            break;
        }
    }
    
    close(socket_fd);
    return 0;
}