#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void get_file(int socket, char* filename) {
    char request[1024];
    sprintf(request, "GET /%s HTTP/1.1\r\n\n", filename);
    
    send(socket, request, strlen(request), 0);
    
    char response[4096];
    recv(socket, response, sizeof(response), 0);
    
    printf("Response from server:\n%s\n", response);
}

void send_file(int socket, char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }

    char file_content[2048];
    char temp[1024];
    file_content[0] = '\0';
    
    while(fgets(temp, 1024, file)) {
        strcat(file_content, temp);
    }
    fclose(file);

    char request[4096];
    sprintf(request, 
        "POST / HTTP/1.1\r\n"
        "filename=%s\r\n"
        "\r\n"
        "%s", 
        filename, file_content);

    send(socket, request, strlen(request), 0);
    
    char response[1024];
    recv(socket, response, sizeof(response), 0);
    printf("Server response:\n%s\n", response);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <ip> <command> <filename>\n", argv[0]);
        printf("Commands: get, send\n");
        return 1;
    }

    char* address = argv[1];
    char* command = argv[2];
    char* filename = argv[3];

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in remote_address;
    remote_address.sin_family = AF_INET;
    remote_address.sin_port = htons(8001);
    inet_aton(address, &remote_address.sin_addr);

    connect(client_socket, (struct sockaddr*)&remote_address, sizeof(remote_address));

    if (strcmp(command, "get") == 0) {
        get_file(client_socket, filename);
    }
    else if (strcmp(command, "send") == 0) {
        send_file(client_socket, filename);
    }
    else {
        printf("Unknown command: %s\n", command);
    }

    close(client_socket);
    return 0;
}
