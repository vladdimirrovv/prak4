#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define MAX_ATTEMPTS 10
#define RANGE_MIN 1
#define RANGE_MAX 100

void handle_client(int client_sock) {
    srand(time(NULL));
    int secret = rand() % RANGE_MAX + RANGE_MIN;
    char buffer[1024];
    int attempts = 0;

    while (attempts < MAX_ATTEMPTS) {
        memset(buffer, 0, sizeof(buffer));
        recv(client_sock, buffer, sizeof(buffer), 0);
        
        int guess = atoi(buffer);
        attempts++;

        if (guess < secret) {
            strcpy(buffer, "Higher!");
        } else if (guess > secret) {
            strcpy(buffer, "Lower!");
        } else {
            strcpy(buffer, "Correct! You won!");
            send(client_sock, buffer, strlen(buffer), 0);
            break;
        }
        send(client_sock, buffer, strlen(buffer), 0);
    }

    if (attempts >= MAX_ATTEMPTS) {
        sprintf(buffer, "You lose! The number was %d", secret);
        send(client_sock, buffer, strlen(buffer), 0);
    }
    close(client_sock);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);
    printf("Server started on port %d...\n", PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
        printf("Client connected!\n");
        handle_client(client_sock);
    }

    close(server_sock);
    return 0;
}