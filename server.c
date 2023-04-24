#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>

#define MAX_CLIENTS 10
#define PORT 5000
#define FURL "keys.txt"

FILE *fp;

void sigintHandler(int sig_num)
{
	fclose(fp);
	printf("\nClosed\n");
	exit(EXIT_SUCCESS);
}

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

int playGame(int p1_fd, int p2_fd)
{
	char ma[1024] = {0};
	char mb[1024] = {0};
	int bytes_read = read(p1_fd, ma, 1024);
	if (bytes_read < 0)
	{
		perror("Read error");
		return -1;
	}
	bytes_read = read(p2_fd, mb, 1024);
	if (bytes_read < 0)
	{
		perror("Read error");
		return -1;
	}
	int a = atoi(ma), b = atoi(mb);
	printf("Player 1: %d and Player 2: %d\n", a, b);
	int res = play(a, b);
	char c[10];
	sprintf(c, "1%d%d", res, b);
	printf("Result %s\n", c);
	send(p1_fd, c, strlen(c), 0);
	sprintf(c, "2%d%d", res, a);
	send(p2_fd, c, strlen(c), 0);
	close(p1_fd);
	close(p2_fd);
	return 0;
}

int login(int p_fd)
{
	char data[41];
	if (read(p_fd, data, 41) < 0)
	{
		perror("Read Error");
		return -1;
	}
	int i = 0;
	char user[20], pass[20];
	char c = data[i];
	while (c != ':')
	{
		c = data[++i];
	}
	for (int j = 0; j < i; j++)
	{
		user[j] = data[j];
	}
	i++;
	for (int j = i; j < strlen(data); j++)
	{
		pass[j - i] = data[j];
	}

	// Check if credentials valid
	char userkey[20], passkey[20];
	while (fscanf(fp, "%20c:%20c", userkey, passkey) != EOF)
	{
		printf("%s:%s", userkey, passkey);
		if (strcmp(user, userkey) == 0 && strcmp(pass, passkey) == 0)
		{
			send(p_fd, "1", strlen("1"), 0);
			return 1;
		}
	}
	send(p_fd, "0", strlen("0"), 0);
	return 1;
}

int signup(int p_fd)
{
	char data[41];
	if (read(p_fd, data, 41) < 0)
	{
		perror("Read Error");
		return -1;
	}
	int i = 0;
	char user[20], pass[20];
	char c = data[i];
	while (c != ':')
	{
		c = data[++i];
	}
	for (int j = 0; j < i; j++)
	{
		user[j] = data[j];
	}
	i++;
	for (int j = i; j < strlen(data); j++)
	{
		pass[j - i] = data[j];
	}

	char userkey[20], passkey[20];
	while (fscanf(fp, "%20c:%20c", userkey, passkey) != EOF)
	{
		if (strcmp(user, userkey) == 0 && strcmp(pass, passkey) == 0)
		{
			send(p_fd, "0", strlen("0"), 0);
			printf("\nExisting user\n");
			return 0;
		}
	}

	fputs(data, fp);
	fputs("\n", fp);
	send(p_fd, "1", strlen("1"), 0);
	return 1;
}

void checklog(int p_fd, int *num_clients, int *client_fds)
{
	char data[41];
	if (read(p_fd, data, 41) < 0)
	{
		perror("Read Error");
		return;
	}
	if (strlen(data) > 2)
	{
		int t = data[0] - '0';
		if (t == 1)
		{
			login(p_fd);
		}
		else
		{
			signup(p_fd);
		}
		for (int i = 0; i < *num_clients; i++)
		{
			if (client_fds[i] == p_fd)
			{
				for (int j = i; j < *num_clients - 1; j++)
				{
					client_fds[j] = client_fds[j + 1];
				}
			}
		}
		*num_clients--;
	}
}

int main(int argc, char const *argv[])
{

	signal(SIGINT, sigintHandler);

	fp = fopen(FURL, "ab");
	int server_fd, client_fds[MAX_CLIENTS], num_clients = 0;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	// Create the server socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Bind the socket to the specified port
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// Listen for incoming connections
	if (listen(server_fd, MAX_CLIENTS) < 0)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}

	// Accept incoming connections
	while (1)
	{
		// Wait for a new connection
		int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
		if (new_socket < 0)
		{
			perror("accept failed");
			exit(EXIT_FAILURE);
		}

		// Add the new client to the list
		client_fds[num_clients++] = new_socket;
		checklog(new_socket, &num_clients, client_fds);

		// If there are two clients waiting, pair them up and play the game
		if (num_clients == 2)
		{
			int player1_fd = client_fds[0];
			int player2_fd = client_fds[1];

			// Play the game
			if (playGame(player1_fd, player2_fd) < 0)
				return -1;

			// Remove the players from the list
			for (int i = 0; i < num_clients; i++)
			{
				if (client_fds[i] == player1_fd || client_fds[i] == player2_fd)
				{
					for (int j = i; j < num_clients - 1; j++)
					{
						client_fds[j] = client_fds[j + 1];
					}
				}
			}
			num_clients -= 2;
		}
	}
	return 0;
}