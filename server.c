#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAX_CLIENTS 10

int play(int a, int b)
{
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

	switch (a)
	{
	case 1:
	{
		switch (b)
		{
		case 1:
			c = 3;
			break;
		case 2:
			c = 2;
			break;
		case 3:
			c = 1;
			break;
		default:
			c = -1;
			break;
		}
	}
	break;
	case 2:
	{
		switch (b)
		{
		case 1:
			c = 1;
			break;
		case 2:
			c = 3;
			break;
		case 3:
			c = 2;
			break;
		default:
			c = -1;
			break;
		}
	}
	break;
	case 3:
	{
		switch (b)
		{
		case 1:
			c = 2;
			break;
		case 2:
			c = 1;
			break;
		case 3:
			c = 3;
			break;
		default:
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

int main()
{
	int client_sockets[MAX_CLIENTS];
	fd_set readfds;
	int max_sd, activity, new_socket, i, valread, sd, player_count = 0;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
	char player_choice[MAX_CLIENTS][10];
	int player_scores[MAX_CLIENTS] = {0};
	char *choices[] = {"rock", "paper", "scissors"};

	// Create ten sockets and add their file descriptors to the array
	for (i = 0; i < MAX_CLIENTS; i++)
	{
		client_sockets[i] = socket(AF_INET, SOCK_STREAM, 0);
		if (client_sockets[i] < 0)
		{
			perror("socket creation failed");
			exit(EXIT_FAILURE);
		}

		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(5000 + i);

		if (bind(client_sockets[i], (struct sockaddr *)&address, sizeof(address)) < 0)
		{
			perror("bind failed");
			exit(EXIT_FAILURE);
		}
		if (listen(client_sockets[i], 3) < 0)
		{
			perror("listen");
			exit(EXIT_FAILURE);
		}
	}

	while (1)
	{
		FD_ZERO(&readfds);
		for (i = 0; i < MAX_CLIENTS; i++)
		{
			FD_SET(client_sockets[i], &readfds);
		}
		max_sd = client_sockets[0];
		for (i = 1; i < MAX_CLIENTS; i++)
		{
			if (client_sockets[i] > max_sd)
			{
				max_sd = client_sockets[i];
			}
		}

		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
		if ((activity < 0) && (errno != EINTR))
		{
			printf("select error");
		}

		for (i = 0; i < MAX_CLIENTS; i++)
		{
			sd = client_sockets[i];
			if (FD_ISSET(sd, &readfds))
			{
				if ((new_socket = accept(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
				{
					perror("accept");
					exit(EXIT_FAILURE);
				}

				valread = read(new_socket, buffer, 1024);
				strcpy(player_choice[i], buffer);
				player_count++;

				if (player_count == 2)
				{
					int p1 = i - 1;
					int p2 = i;
					int a = atoi(player_choice[p1]), b = atoi(player_choice[p2]);
					printf("Player 1: %d and Player 2: %d\n", a, b);
					int res = play(a, b);
					char c[10];
					sprintf(c, "%d", res);
					printf("Result %s\n", c);
					player_count = 0;
					write(client_sockets[p1], c, strlen(c));
					write(client_sockets[p2], c, strlen(c));
					memset(player_choice[p1], 0, sizeof(player_choice[p1]));
					memset(player_choice[p2], 0, sizeof(player_choice[p2]));
				}
			}
		}
	}
	return 0;
}
