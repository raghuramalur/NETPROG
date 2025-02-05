#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    FILE* html_data;
    html_data = fopen("index.html", "r");

    char response_data[1024];
    char temp[1024];
    response_data[0] = '\0';
    while (fgets(temp, 1024, html_data)) {
        strcat(response_data, temp);
    }
    fclose(html_data);

    char http_header[2048] = "HTTP/1.1 200 OK\r\n\n";
    strcat(http_header, response_data);

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8001);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    listen(server_socket, 5);

    int client_socket;
    while (1) {
        client_socket = accept(server_socket, NULL, NULL);

        char request[4096];
        recv(client_socket, request, sizeof(request), 0);

        // Check if request is POST
        if (request[0] == 'P' && request[1] == 'O' && request[2] == 'S' && request[3] == 'T') {
            FILE* upload_file = fopen("uploaded.html", "w");

            // Find start of actual content
            int i, found = 0;
            for (i = 0; i < sizeof(request) - 4; i++) {
                if (request[i] == '\r' && request[i+1] == '\n' &&
                    request[i+2] == '\r' && request[i+3] == '\n') {
                    found = i + 4;
                    break;
                }
            }

            if (found) {
                fputs(request + found, upload_file);
            }
            fclose(upload_file);

            char post_response[] = "HTTP/1.1 200 OK\r\n\nFile Uploaded Successfully";
            send(client_socket, post_response, strlen(post_response), 0);
        } else {
            send(client_socket, http_header, strlen(http_header), 0);
        }

        close(client_socket);
    }

    return 0;
}
