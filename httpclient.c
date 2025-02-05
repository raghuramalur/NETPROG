#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    char buffer[4096], response[8192];
    FILE *file;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address = {AF_INET, htons(8001), INADDR_ANY};

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(server_socket, 5);

    printf("Server running at http://127.0.0.1:8001\n");

    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        recv(client_socket, buffer, sizeof(buffer), 0);

        if (strncmp(buffer, "GET /", 5) == 0) {
            char filename[100] = "index.html";
            sscanf(buffer, "GET /%s", filename);
            if (strcmp(filename, "/") == 0) strcpy(filename, "index.html");

            file = fopen(filename, "r");
            if (file) {
                fread(response, 1, sizeof(response), file);
                fclose(file);
                sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n%s", response);
            } else {
                sprintf(buffer, "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>");
            }
        } else if (strncmp(buffer, "POST /", 6) == 0) {
            char filename[100], *content;
            sscanf(buffer, "POST /%s", filename);
            content = strstr(buffer, "\r\n\r\n") + 4;  // Extract content after headers

            file = fopen(filename, "w");
            if (file) {
                fputs(content, file);
                fclose(file);
                sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>File saved!</h1>");
            } else {
                sprintf(buffer, "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n<h1>File not saved!</h1>");
            }
        }

        send(client_socket, buffer, strlen(buffer), 0);
        close(client_socket);
    }

    return 0;
}
