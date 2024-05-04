#include "spectator.h"

int main(int argc, char** argv) {
	socket_t socket;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s <ServerIP> <ServerPort>\n", argv[0]);
		return 1;
	}

	// Connecting to the server
	socket = connect_to(argv[1], atoi(argv[2]));

	// Authenticating
	authenticate(socket);

	//TODO: Allow for asking the list of courts, and subscribe to one
	//TODO: If one court is selected, listen for SCORE requests and display them

	// Closing socket
	close(socket.file_descriptor);

	return 0;
}

/**
 * @brief Authenticates the spectator
 * @param socket Server socket
 */
void authenticate(socket_t socket) {
	message_t message;
	char data[2];

	sprintf(data, "%d", SPECTATOR_AUTH); // Converting SPECTATOR_AUTH to a string

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