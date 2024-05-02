#include "court.h"

int main(int argc, char** argv) {
	socket_t server_socket, listen_socket, player1, player2;
	pthread_t p1_thread, p2_thread;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s <ServerIP> <ServerPort>\n", argv[0]);
		return 1;
	}

	// Connecting to the server
	server_socket = connect_to(argv[1], atoi(argv[2]));

	// Authenticating
	authenticate(server_socket);

	// Opening listen socket
	listen_socket = create_listen_socket("0.0.0.0", 0);

	// Sending listen port to server
	send_listen_port(server_socket, ntohs(((struct sockaddr_in*)&listen_socket.local_address)->sin_port));

	// Waiting for incoming connections (2 players)
	player1 = accept_client(listen_socket);
	player2 = accept_client(listen_socket);



	return 0;
}

void authenticate(socket_t socket) {
	message_t message;
	char data[2];

	sprintf(data, "%d", COURT_AUTH); // Converting SPECTATOR_AUTH to a string

	// Preparing the authentication message
	prepare_message(&message, AUTH, data);

	// Sending the message
	send_message(&socket, &message, serialize_message);

	// Waiting for the OK response
	receive_message(&socket, &message, deserialize_message);

	if (message.code == (char) OK)
		printf("Authenticated successfully\n");
	else
		printf("Authentication failed\n");
}

void send_listen_port(socket_t socket, int port) {
	message_t message;
	char data[6];

	sprintf(data, "%d", port); // Converting port to a string

	// Preparing the message
	prepare_message(&message, LISTEN_PORT, data);

	// Sending the message
	send_message(&socket, &message, serialize_message);
}

/**
 * @brief Thread for the player
 * @param player_socket: Player socket
 */
void player_thread(void* player_socket) {
	socket_t* player = (socket_t*) player_socket;
	message_t received_msg;

	// Waiting for any score-increment update
	receive_message(player, &received_msg, deserialize_message);

	if (received_msg.code == (char) INCREMENT_SCORE) {
		printf("Score incremented\n");
	}
}