#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
     
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
   
    memset(&serv_addr, '0', sizeof(serv_addr));
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "172.18.2.2", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while(1){
        printf("Enter your choice (rock[1], paper[2], or scissors[3]): ");
        fgets(buffer, 1024, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        if (strcmp(buffer, "1") == 0 || strcmp(buffer, "2") == 0 || strcmp(buffer, "3") == 0) {
            break;
        }
        printf("Invalid input!\n");
    }
    send(sock, buffer, strlen(buffer), 0);
    char response[1024] = {0};
    valread = read(sock, response, 1024);
    if (valread < 0){
        perror("Response error");
        return -1;
    }
    printf("The result is %s\n", response);
    close(sock);
    return 0;
}
