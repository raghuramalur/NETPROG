#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <server_ip> <GET/POST> [file_to_post]\n", argv[0]);
        return 1;
    }

    char* address = argv[1];
    char* method = argv[2];

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in remoteaddress;
    remoteaddress.sin_family = AF_INET;
    remoteaddress.sin_port = htons(8001);
    inet_aton(address, &remoteaddress.sin_addr);

    connect(client_socket, (struct sockaddr *)&remoteaddress, sizeof(remoteaddress));

    char request[4096];
    char response[4096];

    if (strcmp(method, "GET") == 0) {
        strcpy(request, "GET / HTTP/1.1\r\n\n");
        send(client_socket, request, strlen(request), 0);
    } 
    else if (strcmp(method, "POST") == 0 && argc == 4) {
        FILE* file = fopen(argv[3], "r");
        if (!file) {
            printf("File not found!\n");
            return 1;
        }

        char file_data[2048] = "";
        char temp[1024];
        while (fgets(temp, sizeof(temp), file)) {
            strcat(file_data, temp);
        }
        fclose(file);

        sprintf(request, "POST / HTTP/1.1\r\nContent-Length: %lu\r\n\r\n%s", strlen(file_data), file_data);
        send(client_socket, request, strlen(request), 0);
    } 
    else {
        printf("Invalid method. Use GET or POST.\n");
        return 1;
    }

    recv(client_socket, response, sizeof(response), 0);
    printf("Response from server:\n%s\n", response);

    close(client_socket);
    return 0;
}
