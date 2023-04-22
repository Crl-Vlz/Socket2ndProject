#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_ADDRESS "172.18.2.2"
#define SERVER_PORT 5000
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        printf("Error: could not create socket\n");
        exit(EXIT_FAILURE);
    }

    // set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_addr.sin_port = htons(SERVER_PORT);

    // connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Error: could not connect to server\n");
        exit(EXIT_FAILURE);
    }

    // get player's choice
    char choice[1];
    int num = -1;
    while(1 == 1){
        printf("Enter your choice (rock[1], paper[2], or scissors[3]): ");
        fgets(choice, 1, stdin);
        int num = atoi(choice[0]);
        if (num == 1 || num == 2 || num == 3)
            break;
    }

    choice[strcspn(choice, "\n")] = '\0';  // remove newline character

    // send player's choice to server
    send(client_socket, choice, strlen(choice), 0);

    // receive result from server
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
        printf("Error: could not receive data from server\n");
        exit(EXIT_FAILURE);
    }

    // print result
    buffer[bytes_received] = '\0';  // add null terminator to received data
    int out = atoi(buffer);
    switch(out){
        case 1:
            printf("Player A wins!\n");
            break;
        case 2:
            printf("Player B wins!\n");
            break;
        case 3:
            printf("Tie\n");
            break;
        case -1:
            printf("Communication error\n");
            break;
    }

    // close socket
    close(client_socket);

    return 0;
}