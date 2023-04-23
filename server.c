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
	sprintf(c, "%d", res);
	printf("Result %s\n", c);
	sprintf(c, "1%d", res);
	printf("Player A gets %s\n", c);
	send(p1_fd, c, strlen(c), 0);
	sprintf(c, "2%d", res);
	printf("Player B gets %s\n", c);
	send(p2_fd, c, strlen(c), 0);
	close(p1_fd);
	close(p1_fd);
	return 0;
}

int randomValue(int lower, int upper){
	return (rand() % (upper - lower + 1)) + lower;
}

int main(int argc, char const *argv[])
{
	srand(time(NULL));
	int tcp_socket, udp_socket, client_fds[MAX_CLIENTS], num_clients = 0, client_len;
	struct sockaddr_in tcp_address, udp_address, client_address;
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
	int max_fd = (tcp_socket > udp_socket) ? tcp_socket : udp_socket, activity;

	// Clear file descriptor
	FD_ZERO(&readfds);

	// Add TCP and UDP ports to file descriptors
	FD_SET(tcp_socket, &readfds);
	FD_SET(udp_socket, &readfds);

	// Accept incoming connections
	while (1)
	{

		if((activity = select(max_fd + 1, &readfds, NULL, NULL, NULL)) < 0 && (errno != EINTR)){
			perror("Failed to select sockets");
			exit(EXIT_FAILURE);
		}

		if(FD_ISSET(tcp_socket, &readfds)){
			int p1_fd, p2_fd;
			if((p1_fd = accept(tcp_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_len)) < 0){
				perror("Failure connecting to client 1");
				exit(EXIT_FAILURE);
			}
			if((p2_fd = accept(tcp_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_len)) < 0){
				perror("Failure connecting to client 2");
				exit(EXIT_FAILURE);
			}
			if (playGame(p1_fd, p2_fd) < 0){
				perror("Error while playing game");
				exit(EXIT_FAILURE);
			}
		}

		if (FD_ISSET(udp_socket, &readfds)){
			printf("Activity from UDP socket");
			if (recvfrom(udp_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_address, (struct socklen_t *)&client_len) < 0){
				perror("Error receiving UDP packet");
				exit(EXIT_FAILURE);
			}
			printf("Recieved %s from UDP packet", buffer);
			int res = play(atoi(buffer), randomValue(1, 3));
			snprintf(buffer, BUFFER_SIZE, "%d", res);
			if (sendto(udp_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&client_address, client_len) < 0){
				perror("Error sending UDP packet");
				exit(EXIT_FAILURE);
			}
		}
	}
	return 0;
}