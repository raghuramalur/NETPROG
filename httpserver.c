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

    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    listen(server_socket, 5);

    int client_socket;
    char request[8192];  // Increased buffer size to handle larger requests

    while (1) {
        client_socket = accept(server_socket, NULL, NULL);
        recv(client_socket, request, sizeof(request) - 1, 0);

        // Check if it's a POST request
        if (strncmp(request, "POST", 4) == 0) {
            FILE* upload_file = fopen("uploaded.html", "w");

            // Find the beginning of the actual HTML content
            char* body = strstr(request, "\r\n\r\n");
            if (body) {
                body += 4;  // Move pointer to skip "\r\n\r\n"
                fputs(body, upload_file);
            }
            fclose(upload_file);

            // Send a response back to the client
            char post_response[] = "HTTP/1.1 200 OK\r\n\nFile Uploaded Successfully";
            send(client_socket, post_response, strlen(post_response), 0);
        } else {
            // Handle GET request
            send(client_socket, http_header, strlen(http_header), 0);
        }

        close(client_socket);
    }

    return 0;
}
