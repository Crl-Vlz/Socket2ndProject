#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define TCP_PORT 5000
#define UDP_PORT 5001
#define IP_ADDR "172.18.2.2"

char *parse_player(char p){
    int player = atoi(&p);
    switch(player){
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

    send(sock, message, 1024, 0);
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

void handle_udp_message(char *message)
{
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socker creation error\n");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(UDP_PORT);

    if (inet_pton(AF_INET, IP_ADDR, &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address\n");
        exit(EXIT_FAILURE);
    }

    sendto(sock, message, 1024, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (recvfrom(sock, message, 1024, 0, NULL, NULL) < 0)
    {
        perror("Could not get UDP packet\n");
        exit(EXIT_FAILURE);
    }
    printf("%s", parse_player(message[0]));
    printf("%s", parse_result(message[1]));
    close(sock);
}

int main()
{
    struct sockaddr_in address;
    int sock = 0, udp_sock, valread;
    struct sockaddr_in serv_addr, udp_addr;
    char buffer[1024] = {0};

    int option = -1;
    do
    {
        printf("Select the option\n TCP: 1 \n UDP: 2 \n Exit: 3\n");
        fgets(buffer, 1024, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        option = atoi(buffer);
        switch (option)
        {
        case 1:
        {
            handle_tcp_message();
        }
        break;
        case 2:
        {
            // UDP code goes here
            while (1)
            {
                printf("Enter your choice (rock[1], paper[2], or scissors[3]):\n");
                fgets(buffer, 1024, stdin);
                buffer[strcspn(buffer, "\n")] = 0;
                if (strcmp(buffer, "1") == 0 || strcmp(buffer, "2") == 0 || strcmp(buffer, "3") == 0)
                {
                    break;
                }
                perror("Invalid input!\n");
            }
            handle_udp_message(buffer);
        }
        break;
        case 3:
        {
            // Exit program
            printf("Goodbye\n");
        }
        break;
        default:
        {
            perror("Invalid input\n");
        }
        break;
        }
    } while (option != 3);
    return 0;
}
