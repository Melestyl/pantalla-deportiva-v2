#include "player.h"

int main(int argc, char** argv) {
	socket_t socket;
	buffer_t first_name, last_name;
	char choice;
	message_t received_message;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s <ServerIP> <ServerPort>\n", argv[0]);
		return 1;
	}

	// Connecting to the server
	socket = connect_to(argv[1], atoi(argv[2]));

	// Asking the player for their first and last name
	printf("Entrez votre prénom : ");
	fgets(first_name, sizeof(buffer_t), stdin);
	first_name[strlen(first_name) - 1] = '\0'; // Removing the newline character

	printf("Entrez votre nom : ");
	fgets(last_name, sizeof(buffer_t), stdin);
	last_name[strlen(last_name) - 1] = '\0'; // Removing the newline character

	printf("Que voulez-vous faire ?\n"
	       "1. Etre invité par votre partenaire\n"
	       "2. Inviter votre partenaire\n"
	       "Votre choix : ");
	scanf("%c", &choice);

	// Authenticating as an invited or inviting player
	switch (choice) {
		case '1':
			// Authenticating as an invited player
			authenticate(socket, INVITED_AUTH, first_name, last_name);

			// Getting the player's ID as a response
			receive_message(&socket, &received_message, deserialize_message);
			if (received_message.code == INFO_PLAYER)
				printf("Votre ID est : %s\n", received_message.data);
			else {
				printf("Erreur lors de la réception de l'ID\n");
				return 1;
			}
			break;
		case '2':
			// Authenticating as an inviting player
			authenticate(socket, HOST_AUTH, first_name, last_name);
			break;
		default:
			fprintf(stderr, "Choix invalide\n");
			exit(1);
	}

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
	sprintf(data, "%d:%s:%s", type, last_name, first_name);

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