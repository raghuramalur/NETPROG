#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);  // Changed to SOCK_STREAM for TCP

    struct sockaddr_in socket_address;
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(9002);
    socket_address.sin_addr.s_addr = INADDR_ANY;

    if (connect(socket_fd, (struct sockaddr*)&socket_address, sizeof(socket_address)) == -1) {
        perror("Connection failed");
        return 1;
    }

    char sendbuffer[256];
    char recvbuffer[256];

    while (1) {
        printf("Enter numbers separated by space (or type 'exit' to quit):\n");
        fgets(sendbuffer, 256, stdin);

        // If user enters "exit", stop communication
        if (strncmp(sendbuffer, "exit", 4) == 0) {
            break;
        }

        send(socket_fd, sendbuffer, strlen(sendbuffer), 0);

        // Receive result from server
        recv(socket_fd, recvbuffer, sizeof(recvbuffer), 0);
        printf("Result from server: %s\n", recvbuffer);

        memset(recvbuffer, 0, sizeof(recvbuffer));
        memset(sendbuffer, 0, sizeof(sendbuffer));
    }

    close(socket_fd);
    return 0;
}
