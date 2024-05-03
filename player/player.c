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

			// Invited player function
			invited_player(socket);
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

/**
 * @fn void wait_for_partner(socket_t socket)
 * @brief Function for the player waiting for an invitation
 * @param socket: Server socket
 */
void wait_for_partner(socket_t socket) {
	message_t received_msg, send_msg;
	buffer_t last_name, first_name;
	char* save_ptr;
	int choice;

	do {
		printf("En attente d'une invitation...\n");

		// Waiting to receive an INVITE from the server
		receive_message(&socket, &received_msg, deserialize_message);
		if (received_msg.code != INVITE) {
			printf("Erreur lors de la réception de l'invitation\n");
			exit(1);
		}

		// Getting the first name and last name of the inviting player
		strcpy(last_name, strtok_r(received_msg.data, ":", &save_ptr));
		strcpy(first_name, strtok_r(NULL, ":", &save_ptr));

		// Asking for validation
		printf("Invitation reçue de la part de '%s %s'\n"
		       "1) Accepter\n"
		       "2) Refuser et attendre une invitation\n"
		       ": ", first_name, last_name);

		// Getting user's choice
		scanf("%d", &choice);

		if (choice == 1) {
			printf("Invitation acceptée\n");

			// Sending response to the server
			prepare_message(&send_msg, (char) OK, "");
			send_message(&socket, &send_msg, serialize_message);
		}
		else {
			printf("Invitation refusée\n");

			// Sending response to the server
			prepare_message(&send_msg, (char) NOK, "");
			send_message(&socket, &send_msg, serialize_message);
		}
	} while (choice != 1);
}

/**
 * @fn void invited_player(socket_t socket)
 * @brief Function for the invited player
 * @param socket: Server socket
 */
void invited_player(socket_t socket) {
	message_t received_msg;

	// Getting the player's ID as a response
	receive_message(&socket, &received_msg, deserialize_message);
	if (received_msg.code == INFO_PLAYER)
		printf("Votre ID est : %s\n", received_msg.data);
	else {
		printf("Erreur lors de la réception de l'ID\n");
		exit(1);
	}

	// Waiting for a partner
	wait_for_partner(socket);

	//TODO: Match mode
}