#include "player.h"

int main(int argc, char** argv) {
	socket_t socket, court_socket;
	buffer_t first_name, last_name;
	char choice;

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

			// Inviting a partner
			invite_partner(socket);
			break;

		default:
			fprintf(stderr, "Choix invalide\n");
			exit(1);
	}

	// Connecting to the court
	printf("Attente de la réception du court...\n");
	court_socket = connect_to_court(&socket);

	// Entering match mode
	match_mode(&court_socket);

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
			fprintf(stderr, "Erreur lors de la réception de l'invitation\n");
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
		fprintf(stderr, "Erreur lors de la réception de l'ID\n");
		exit(1);
	}

	// Waiting for a partner
	wait_for_partner(socket);
}

/**
 * @fn void print_list_of_players(char* data)
 * @brief Prints the list of players in a beautiful way
 * @param data: Data received from the server (formatted list of players)
 */
void print_list_of_players(char* data) {
	char *save_ptr, *number, *last_name, *first_name;

	if (data == NULL) {
		printf("Aucun joueur n'est connecté\n");
		return;
	}

	do {
		number = strtok_r(data, ":", &save_ptr);
		last_name = strtok_r(NULL, ":", &save_ptr);
		first_name = strtok_r(NULL, ":", &save_ptr);
		printf("-> [%s] %s %s\n", number, last_name, first_name);
	} while (number != NULL && last_name != NULL && first_name != NULL);
}

/**
 * @fn void invite_partner(socket_t socket)
 * @brief Function to invite a partner
 * @param socket: Server socket
 */
void invite_partner(socket_t socket) {
	message_t send_msg, received_msg;
	buffer_t data;
	int choice, partner_found = 0;

	do {
		// Asking for who to invite
		printf("Qui voulez-vous inviter ? (numéro de joueur)\n"
		       "(0 pour afficher la liste des joueurs)\n\n"
		       "Numéro : ");

		// Getting user's choice
		scanf("%d", &choice);

		// Printing the list of players if asked
		if (choice == 0) {
			// Sending the request
			prepare_message(&send_msg, ASK_PLAYERS, "");
			send_message(&socket, &send_msg, serialize_message);

			// Receiving the list of players
			receive_message(&socket, &received_msg, deserialize_message);
			if (received_msg.code == LIST_PLAYERS) {
				//print_list_of_players(received_msg.data);
				printf("Liste des joueurs : %s\n", received_msg.data);
			}
			else {
				fprintf(stderr, "Erreur lors de la réception de la liste des joueurs\n");
				fprintf(stderr, "reçu : %d\n", received_msg.code);
			}
		}

		// Inviting the player
		else if (choice != 0) {
			// Sending the invitation
			sprintf(data, "%d", choice); // Player's ID
			prepare_message(&send_msg, PLAY_WITH, data);
			send_message(&socket, &send_msg, serialize_message);
			printf("Invitation envoyée, en attente de validation\n");

			// Waiting for the response of the player
			receive_message(&socket, &received_msg, deserialize_message);
			if (received_msg.code == (char) OK) {
				printf("Invitation acceptée par le joueur !\n");
				partner_found = 1;
			}
			else
				printf("Le partenaire a refusé ou n'existe pas.\nVeuillez choisir un autre partenaire\n");
		}
	} while (!partner_found);
}

/**
 * @fn socket_t connect_to_court()
 * @brief Connects to a court given by the server
 * @param socket: Server socket
 * @return Court socket
 */
socket_t connect_to_court(socket_t* socket) {
	message_t received_msg;
	buffer_t court_ip;
	int court_port;
	char* save_ptr;

	// Receiving the court
	sleep(1);
	receive_message(socket, &received_msg, deserialize_message);
	if (received_msg.code == COURT_FOUND)
		printf("Court trouvé !\n");
	else {
		fprintf(stderr, "Erreur lors de la réception du court\n");
		fprintf(stderr, "reçu : %d\n", received_msg.code);
		exit(1);
	}

	// Retrieving the court's IP and port
	strcpy(court_ip, strtok_r(received_msg.data, ":", &save_ptr));
	court_port = atoi(strtok_r(NULL, ":", &save_ptr));

	// Creating a new socket to connect to the court
	return connect_to(court_ip, court_port);
}

/**
 * @fn void match_mode(socket_t court_socket)
 * @brief Enters the match mode
 * @param court_socket: Court socket
 */
void match_mode(socket_t* court_socket) {
	message_t send_msg, received_msg;

	printf("Appuyez sur Entrée pour ajouter un point au score !\n");

	getchar(); // Removing the newline character from the buffer
	while (1) {
		// Waiting for the player to press Enter
		getchar();
		printf("Ajout d'un point...\n");

		// Sending the point
		printf("Envoi du message au terrain\n");
		prepare_message(&send_msg, INCREMENT_SCORE, "");
		send_message(court_socket, &send_msg, serialize_message);
		sleep(1);

		// Waiting for an OK
		printf("Attente de la réponse du terrain\n");
		receive_message(court_socket, &received_msg, deserialize_message);
		if (received_msg.code == (char) OK)
			printf("Point ajouté !\n");
		else if (received_msg.code == (char) END_MATCH) {
			printf("Fin du match !\n");
			break;
		}
		else {
			fprintf(stderr, "Erreur lors de l'ajout du point\n");
			break;
		}
		sleep(1);
	}
}