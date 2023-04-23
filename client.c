#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define TCP_PORT 5000
#define IP_ADDR "172.18.2.2"

char *parse_player(char p)
{
    int player = atoi(&p);
    switch (player)
    {
    case 1:
        return "You are Player A\n";
        break;
    case 2:
        return "You are Player B\n";
        break;
    default:
        return "Could not define player\n";
        break;
    }
}

char *parse_result(char r)
{
    int code = atoi(&r);
    switch (code)
    {
    case 1:
        return "Player A wins!\n";
    case 2:
        return "Player B wins!\n";
    case 3:
        return "Its a tie!\n";
    case 0:
    default:
        return "Communication error\n";
    }
}

void handle_tcp_message()
{
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error\n");
        close(sock);
        return;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TCP_PORT);

    if (inet_pton(AF_INET, IP_ADDR, &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address\n");
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection failed\n");
        close(sock);
        return;
    }

    char message[1024];
    while (1)
    {
        printf("Enter your choice (rock[1], paper[2], or scissors[3]):\n");
        fgets(message, 1024, stdin);
        message[strcspn(message, "\n")] = 0;
        if (strcmp(message, "1") == 0 || strcmp(message, "2") == 0 || strcmp(message, "3") == 0)
        {
            break;
        }
        perror("Invalid input!\n");
    }

    if(send(sock, message, 1024, 0) < 0){
        perror("Error sending data\n");
        close(sock);
        return;
    }
    if (read(sock, message, 1024) < 0)
    {
        perror("Read error");
        close(sock);
        return;
    }
    char p = *(message), r = *(message + 1);
    printf("%s", parse_player(p));
    printf("%s", parse_result(r));
    close(sock);
}

int main() {
    char buffer[1024] = {0};

    int option = -1;
    do{
        printf("1: Play\n2: Rules\n3: Exit\n");
        fgets(buffer, 1024, stdin);
        option = atoi(buffer);
        switch(option){
            case 1 : {
                handle_tcp_message();
            }
            break;
            case 2 :{
                printf("This program connects you with other players to have some ");
                printf("intense round of rock paper scissors!\nTo start just select if you want");
                printf("to play against other players or against the machine, then just select what you will\n");
                printf("throw. But, remember to have fun!!!\n");
            }
            break;
            case 3:{
                printf("Goodbye!\n");
            }
        }
    }while(option != 3);
    return 0;
}
