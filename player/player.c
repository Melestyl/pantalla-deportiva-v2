#include "player.h"

int main(int argc, char** argv) {
	socket_t socket;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s <ServerIP> <ServerPort>\n", argv[0]);
		return 1;
	}

	// Connecting to the server
	socket = connect_to(argv[1], atoi(argv[2]));

	// Authenticating
	//TODO: Ask the user for type, their first and last name
	authenticate(socket, INVITED_AUTH, "Tom", "TELLIER--CALOONE");

	// Closing socket
	close(socket.file_descriptor);

	return 0;
}

/**
 * @brief Authenticates the player
 * @param socket: Server socket
 * @param type: Whether the player is inviting or invited
 * @param first_name: Player's first name
 * @param last_name: Player's last name
 */
void authenticate(socket_t socket, char type, char* first_name, char* last_name) {
	message_t message;
	buffer_t data;

	// Preparing the data
	sprintf(data, "%d:%s:%s", type, first_name, last_name);

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