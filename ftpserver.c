// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>

#define BUFFER_SIZE 1024
#define FILE_SIZE 4096

void handle_list_command(int client_fd) {
    DIR *d;
    struct dirent *dir;
    char file_list[BUFFER_SIZE] = "";
    
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            strcat(file_list, dir->d_name);
            strcat(file_list, "\n");
        }
        closedir(d);
    }
    
    send(client_fd, file_list, strlen(file_list), 0);
}

void handle_get_command(int client_fd, char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        char *error = "Error: File not found\n";
        send(client_fd, error, strlen(error), 0);
        return;
    }
    
    char file_buffer[FILE_SIZE];
    size_t bytes_read;
    
    while ((bytes_read = fread(file_buffer, 1, FILE_SIZE, fp)) > 0) {
        send(client_fd, file_buffer, bytes_read, 0);
    }
    
    fclose(fp);
}

void handle_put_command(int client_fd, char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        char *error = "Error: Cannot create file\n";
        send(client_fd, error, strlen(error), 0);
        return;
    }
    
    char file_buffer[FILE_SIZE];
    int bytes_received;
    
    while ((bytes_received = recv(client_fd, file_buffer, FILE_SIZE, 0)) > 0) {
        fwrite(file_buffer, 1, bytes_received, fp);
        if (bytes_received < FILE_SIZE) break;
    }
    
    fclose(fp);
    send(client_fd, "File uploaded successfully\n", 25, 0);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    
    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9002);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    // Bind
    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    
    // Listen
    listen(server_fd, 5);
    printf("FTP Server listening...\n");
    
    while(1) {
        // Accept connection
        client_fd = accept(server_fd, NULL, NULL);
        printf("Client connected!\n");
        
        while(1) {
            memset(buffer, 0, BUFFER_SIZE);
            recv(client_fd, buffer, BUFFER_SIZE, 0);
            
            if (strncmp(buffer, "LIST", 4) == 0) {
                handle_list_command(client_fd);
            }
            else if (strncmp(buffer, "GET", 3) == 0) {
                handle_get_command(client_fd, buffer + 4);
            }
            else if (strncmp(buffer, "PUT", 3) == 0) {
                handle_put_command(client_fd, buffer + 4);
            }
            else if (strncmp(buffer, "QUIT", 4) == 0) {
                printf("Client disconnected\n");
                break;
            }
        }
        
        close(client_fd);
    }
    
    close(server_fd);
    return 0;
}
