#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5000
#define MAXLINE 1024
#define MAXCLIENTS 30

int play(int a, int b) {
	int c = -1;

	/*
		A and B
		1 = Rock
		2 = Paper
		3 = Scissors

		C
		1 = Player A
		2 = Player B
		3 = Tie
		-1 = Error

	*/

	switch(a){
		case 1:{
				switch(b){
					case 1:
						c = 3;
						break;
					case 2:
						c = 2;
						break;
					case 3:
						c = 1;
						break;
					default;
						c = -1;
						break;
				}
			}
			break;
		case 2:{
				switch(b){
					case 1:
						c = 1;
						break;
					case 2:
						c = 3;
						break;
					case 3:
						c = 2;
						break;
					default;
						c = -1;
						break;
				}
			}
			break;
		case 3:{
				switch(b){
					case 1:
						c = 2;
						break;
					case 2:
						c = 1;
						break;
					case 3:
						c = 3;
						break;
					default;
						c = -1;
						break;
				}
			}
			break;
		default:
			c = -1;
			break;
	}

	return c;
}

int main() {
    int sockfd, connfd, num_clients = 0;
    struct sockaddr_in servaddr, cliaddr[MAXCLIENTS];
    char buffer[MAXLINE];
    int n, len;

    // Create a TCP socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize the server address structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, MAXCLIENTS) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (num_clients < MAXCLIENTS) {
            len = sizeof(cliaddr[num_clients]);
            if ((connfd = accept(sockfd, (struct sockaddr *)&cliaddr[num_clients], &len)) < 0) {
                perror("accept failed");
                exit(EXIT_FAILURE);
            }
            num_clients++;
            printf("Player %d connected\n", num_clients);
            if (num_clients % 2 == 0) {
                // Pair up the two players and start a game
                char choices[2][MAXLINE];
                int player1 = num_clients - 1;
                int player2 = num_clients - 2;
                printf("Matched players %d and %d in a game of rock-paper-scissors\n", player1, player2);
                // Receive choices from players
                n = recv(connfd[player1], buffer, MAXLINE, 0);
                if (n > 0) {
                    buffer[n] = '\0';
                    strcpy(choices[1], buffer);
                }
                n = recv(connfd[player2], buffer, MAXLINE, 0);
                if (n > 0) {
                    buffer[n] = '\0';
                    strcpy(choices[0], buffer);
                }
                // Determine winner
                char result[MAXLINE];
                
				//Convert input to numbers
				int a = atoi(&choices[0][0]), b = atoi(&choices[1][0]);
				int c = play(a, b);

				sprintf(result, "%d", c);

                // Send result to players
                send(connfd[player1], result, strlen(result), 0);
                send(connfd[player2], result, strlen(result), 0);
            }
        }
    }

    return 0;
}
