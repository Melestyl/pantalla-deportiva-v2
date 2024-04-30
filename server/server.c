#include "server.h"

int main(int argc, char** argv) {
	int port = 0; // 0 = default for random
	socket_t listen_socket, client_socket;
	message_t message;
	buffer_t buffer;
	char* save_buffer;

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
		printf("Client connected\nIP: %s\nPort: %d\n",
			   inet_ntoa(((struct sockaddr_in*)&client_socket.remote_address)->sin_addr),
					   ntohs(((struct sockaddr_in*)&client_socket.remote_address)->sin_port));

		// Receiving message from the client
		receive_message(&client_socket, &message, deserialize_message);

		switch (message.code) {
			case AUTH:
				strcpy(buffer, message.data);
				strtok_r(buffer, ":", &save_buffer);
				printf("AUTH type: %d\n", atoi(buffer));
				break;
			default:
				printf("Unknown request code received\n");
				break;
		}

		close(client_socket.file_descriptor);
	}

	// Closing socket
	close(listen_socket.file_descriptor);

	return 0;
}