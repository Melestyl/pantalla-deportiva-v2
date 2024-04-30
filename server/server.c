#include "server.h"

void listen_thread(void* socket);

int main(int argc, char** argv) {
	socket_t listen_socket, client_socket;
	char* strtok_pointer;
	message_t message;
	pthread_t thread;
	buffer_t buffer;
	int port = 0; // 0 = default for random

	// Trying to assign the port following user's choice
	if (argc > 1) {
		port = atoi(argv[1]);
		// If the port isn't correct, set it to 0
		if (port < 0 || port > 65535)
			port = 0;
	}

	// Creating STREAM listen socket
	listen_socket = create_listen_socket("0.0.0.0", port);
	printf("Listening on port %d\n", ntohs(((struct sockaddr_in*)&listen_socket.local_address)->sin_port));

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
		fprintf(stderr, "[%s:%d] has sent a non-auth request and is not authentified.\n", ip, port);
		close(client_socket->file_descriptor);
		return;
	}

	// Processing auth
	switch (message.data[0]) {
		// Player who invites
		case '0':
			printf("[%s:%d] is a player who invites.\n", ip, port);
			break;

		// Player who is invited
		case '1':
			printf("[%s:%d] is a player who is invited.\n", ip, port);
			break;

		// Court
		case '2':
			printf("[%s:%d] is a court.\n", ip, port);
			break;

		// Spectator
		case '3':
			printf("[%s:%d] is a spectator.\n", ip, port);
			break;

		// Unknown
		default:
			fprintf(stderr, "[%s:%d] is trying to authenticate with an unknown role.\n", ip, port);
			break;
	}
}