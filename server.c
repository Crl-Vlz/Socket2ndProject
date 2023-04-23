#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>

#define MAX_CLIENTS 10
#define PORT 5000
#define UDP_PORT 5001
#define BUFFER_SIZE 1024

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
		0 = Error

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
			c = 0;
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
			c = 0;
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
			c = 0;
			break;
		}
	}
	break;
	default:
		c = 0;
		break;
	}

	return c;
}

int playGame(char *ma, char *mb)
{
	printf("\nPlaying game\n");
	int a = atoi(ma), b = atoi(mb);
	printf("Player 1: %d and Player 2: %d\n", a, b);
	return play(a, b);
}

int randomValue(int lower, int upper){
	return (rand() % (upper - lower + 1)) + lower;
}

int main(int argc, char const *argv[])
{
	srand(time(NULL));
	int tcp_socket, udp_socket, client_fds[MAX_CLIENTS], num_clients = 0;
	struct sockaddr_in tcp_address, udp_address, client_address;
	socklen_t client_len = sizeof(client_address);
	char buffer[1024];

	// Create the server socket
	if ((tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("failed to make tcp socket");
		exit(EXIT_FAILURE);
	}

	// Create UDP server socket
	if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("failed to make UDP socket");
		exit(EXIT_FAILURE);
	}

	// Initialize TCP socket
	memset(&tcp_address, 0, sizeof(tcp_address));
	tcp_address.sin_family = AF_INET;
	tcp_address.sin_addr.s_addr = INADDR_ANY;
	tcp_address.sin_port = htons(PORT);

	// Initialize UDP socket
	memset(&udp_address, 0, sizeof(udp_address));
	udp_address.sin_family = AF_INET;
	udp_address.sin_addr.s_addr = INADDR_ANY;
	udp_address.sin_port = htons(UDP_PORT);

	// Bind TCP socket
	if (bind(tcp_socket, (struct sockaddr *)&tcp_address, sizeof(tcp_address)) < 0)
	{
		perror("TCP bind failed");
		exit(EXIT_FAILURE);
	}

	// Bind UDP socket
	if (bind(udp_socket, (struct sockaddr *)&udp_address, sizeof(udp_address)) < 0) {
		perror("UDP bind failure");
		exit(EXIT_FAILURE);
	}

	// Listen for incoming connections on TCP socket
	if (listen(tcp_socket, MAX_CLIENTS) < 0)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}

	fd_set readfds;
	int max_fd = (tcp_socket > udp_socket) ? tcp_socket : udp_socket;
	int activity;

	char choices[MAX_CLIENTS][1024];

	// Clear file descriptor
	FD_ZERO(&readfds);

	// Add TCP and UDP ports to file descriptors
	FD_SET(tcp_socket, &readfds);
	FD_SET(udp_socket, &readfds);

	// Accept incoming connections
	while (1)
	{

		if((activity = select(max_fd + 1, &readfds, NULL, NULL, NULL)) < 0){
			perror("Failed to select sockets");
			continue;
		}

		if(FD_ISSET(tcp_socket, &readfds)){
			printf("%d", num_clients);
			printf("Activity in TCP port");
			int p_fd;
			if((p_fd = accept(tcp_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_len)) < 0){
				perror("Failure connecting to client 1\n");
				continue;
			}
			if (read(p_fd, choices[num_clients], 1024) < 0){
				perror("Failure to read\n");
				continue;
			}
			FD_SET(p_fd, &readfds);
			if (p_fd > max_fd)
				max_fd = p_fd;
			client_fds[num_clients++] = p_fd;
			if (num_clients >= 2){
				int c;
				c = playGame(choices[0], choices[1]);
				char ma[10];
				char mb[10];
				sprintf(ma, "1%d", c);
				sprintf(mb, "2%d", c);
				printf("The result is %d\n", c);
				printf("The auth protocol is %s\n", ma);
				send(client_fds[0], ma, strlen(ma), 0);
				send(client_fds[1], mb, strlen(mb), 0);
				FD_CLR(client_fds[0], &readfds);
				FD_CLR(client_fds[1], &readfds);
				close(client_fds[0]);
				close(client_fds[1]);
				for(int i = 2; i < num_clients; i++){
					client_fds[i - 2] = client_fds[i];
					for(int j = 0; j < 1024; j++)
						choices[i - 2][j] = choices[i][j];
				}
				num_clients -= 2;
			}
		}

		if (FD_ISSET(udp_socket, &readfds)){
			if (recvfrom(udp_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_address, (socklen_t *)&client_len) < 0){
				perror("Error receiving UDP packet");
				continue;
			}
			printf("Recieved %s from UDP packet", buffer);
			int res = play(atoi(buffer), randomValue(1, 3));
			snprintf(buffer, BUFFER_SIZE, "%d", res);
			if (sendto(udp_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&client_address, client_len) < 0){
				continue;
			}
		}
	}
	return 0;
}