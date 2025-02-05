#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void get_file(int socket, char *filename) {
    char request[1024];
    sprintf(request, "GET /%s HTTP/1.1\r\n\r\n", filename);
    
    send(socket, request, strlen(request), 0);
    char response[4096];
    recv(socket, response, sizeof(response), 0);
    
    printf("Server Response:\n%s\n", response);
}

void send_file(int socket, char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }

    char file_content[2048], temp[1024];
    file_content[0] = '\0';
    while (fgets(temp, sizeof(temp), file)) strcat(file_content, temp);
    fclose(file);

    char request[4096];
    sprintf(request, "POST /%s HTTP/1.1\r\n\r\n%s", filename, file_content);

    send(socket, request, strlen(request), 0);
    
    char response[1024];
    recv(socket, response, sizeof(response), 0);
    printf("Server Response:\n%s\n", response);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <ip> <get/send> <filename>\n", argv[0]);
        return 1;
    }

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in remote_address = {AF_INET, htons(8001)};
    inet_pton(AF_INET, argv[1], &remote_address.sin_addr);

    connect(client_socket, (struct sockaddr*)&remote_address, sizeof(remote_address));

    if (strcmp(argv[2], "get") == 0) {
        get_file(client_socket, argv[3]);
    } else if (strcmp(argv[2], "send") == 0) {
        send_file(client_socket, argv[3]);
    } else {
        printf("Unknown command\n");
    }

    close(client_socket);
    return 0;
}
