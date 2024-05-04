/**
 * @file server.c
 * @brief Server side of the application.
 * @date 2024-04-30
 */

#include "server.h"
#include "player_functions.h"
#include "court_functions.h"

socket_t listen_socket; // Declared globally to be closed in the signal handler function

int main(int argc, char** argv) {
	socket_t client_socket;
	pthread_t thread;
	int port = 0; // 0 = default for random

	// Trying to assign the port following user's choice
	if (argc > 1) {
		port = atoi(argv[1]);
		// If the port isn't correct, setting it to 0
		if (port < 0 || port > 65535)
			port = 0;
	}

	// Creating STREAM listen socket
	listen_socket = create_listen_socket("0.0.0.0", port);
	printf("Listening on port %d\n", ntohs(((struct sockaddr_in*)&listen_socket.local_address)->sin_port));

	// Setting up signal handler to close the socket properly
	signal(SIGINT, sigint_handler);

	// Accepting clients
	while (1) {
		client_socket = accept_client(listen_socket);
		thread = pthread_create(&thread,
								NULL,
								(void*) listen_thread,
								(void*) &client_socket);
	}

	// Closing socket
	close(listen_socket.file_descriptor);

	return 0;
}

/**
 * @fn void listen_thread(void* socket)
 * @brief Thread to listen to a client.
 * @param socket: client socket created after an accept
 */
void listen_thread(void* socket) {
	socket_t* client_socket = (socket_t *) socket;
	message_t message;
	buffer_t ip;
	int port;

	strcpy(ip, inet_ntoa(((struct sockaddr_in*)&client_socket->remote_address)->sin_addr));
	port = ntohs(((struct sockaddr_in*)&client_socket->remote_address)->sin_port);

	// Receiving message from the client
	receive_message(client_socket, &message, deserialize_message);

	// Rejecting if the client is not trying to authenticate first
	if (message.code != AUTH) {
		fprintf(stderr, "[%s:%d] has sent a non-auth request and is not authenticated.\n", ip, port);
		close(client_socket->file_descriptor);
		return;
	}

	// Processing auth
	switch (message.data[0]) {
		// Player who invites
		case '1':
			printf("[%s:%d] is a player who invites.\n", ip, port);
			break;

		// Player who is invited
		case '2':
			printf("[%s:%d] is a player who is invited.\n", ip, port);
			invited_player(client_socket, message.data);
			break;

		// Court
		case '3':
			printf("[%s:%d] is a court.\n", ip, port);
			new_court(client_socket, ip);
			break;

		// Spectator
		case '4':
			printf("[%s:%d] is a spectator.\n", ip, port);
			spectator_function(client_socket);
			break;

		// Unknown
		default:
			fprintf(stderr, "[%s:%d] is trying to authenticate with an unknown role.\n", ip, port);
			break;
	}
}

/**
 * @fn void sigint_handler(int signum)
 * @brief Signal handler for SIGINT, closing the socket properly
 * @param signum: unused
 */
void sigint_handler(int signum) {
	close(listen_socket.file_descriptor);
	printf("\nServer closed.\n");
	exit(0);
}