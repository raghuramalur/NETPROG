#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int main() {
    char buffer[1024];
    char response[2048];
    char http_header[2048];
    FILE* file;
    int client_count = 0;
    
    // Create socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Define address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8001);
    server_address.sin_addr.s_addr = INADDR_ANY;
    
    // Bind socket
    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
    listen(server_socket, 5);

    printf("Server started on port 8001\n");
    printf("Waiting for clients...\n");

    int client_socket;
    while(1) {
        client_socket = accept(server_socket, NULL, NULL);
        client_count++;
        printf("Connected to Client %d\n", client_count);
        
        // Clear buffers
        memset(buffer, 0, sizeof(buffer));
        memset(response, 0, sizeof(response));
        memset(http_header, 0, sizeof(http_header));
        
        // Receive request
        recv(client_socket, buffer, sizeof(buffer), 0);
        
        // Check if it's GET request
        if (buffer[0] == 'G' && buffer[1] == 'E' && buffer[2] == 'T') {
            // Get filename from request
            char filename[100];
            int i = 5;  // Skip "GET /"
            int j = 0;
            while (buffer[i] != ' ' && buffer[i] != '\0') {
                filename[j] = buffer[i];
                i++;
                j++;
            }
            filename[j] = '\0';
            
            // Try to open file
            file = fopen(filename, "r");
            if (file != NULL) {
                // Read file content
                char temp[1024];
                while(fgets(temp, 1024, file)) {
                    strcat(response, temp);
                }
                fclose(file);
                
                sprintf(http_header, "HTTP/1.1 200 OK\r\n\n%s", response);
                printf("Sent file '%s' to Client %d\n", filename, client_count);
            } else {
                sprintf(http_header, "HTTP/1.1 404 Not Found\r\n\nFile not found!");
                printf("Error: File '%s' not found - Client %d\n", filename, client_count);
            }
        }
        // Check if it's POST request
        else if (buffer[0] == 'P' && buffer[1] == 'O' && buffer[2] == 'S' && buffer[3] == 'T') {
            // Get filename
            char filename[100];
            int i = 0;
            while (buffer[i] != '\n') i++; // Skip first line
            i++;
            
            // Get filename from second line
            int j = 0;
            while (buffer[i] != '\n' && buffer[i] != '\0') {
                filename[j] = buffer[i];
                i++;
                j++;
            }
            filename[j] = '\0';
            
            // Skip to content
            i += 2;  // Skip empty line
            
            // Save content to file
            file = fopen(filename, "w");
            if (file != NULL) {
                fputs(&buffer[i], file);
                fclose(file);
                sprintf(http_header, "HTTP/1.1 200 OK\r\n\nFile saved successfully!");
                printf("Received file '%s' from Client %d\n", filename, client_count);
            } else {
                sprintf(http_header, "HTTP/1.1 500 Error\r\n\nCould not save file!");
                printf("Error: Could not save file '%s' from Client %d\n", filename, client_count);
            }
        }
        
        send(client_socket, http_header, strlen(http_header), 0);
        close(client_socket);
        printf("Client %d disconnected\n", client_count);
        printf("Waiting for clients...\n");
    }
    return 0;
}
