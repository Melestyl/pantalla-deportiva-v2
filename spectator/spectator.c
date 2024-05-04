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
	printf("Authentification en cours...\n");
	authenticate(socket);
	printf("Authentification effectuée !\n");

	// Selecting a court
	select_and_subscribe(socket);

	// If one court is selected, listen for SCORE requests and display them
	listen_for_score(socket);

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

/**
 * @fn void select_and_subscribe(socket_t socket)
 * @brief Selects a court and subscribes to it
 * @param socket Server socket
 */
void select_and_subscribe(socket_t socket) {
	message_t send_msg, received_msg;
	buffer_t data;
	int choice, subscribed = 0;

	do {
		printf("A quel terrain voulez-vous vous abonner ? (numéro de terrain)\n"
			   "(0 pour afficher la liste des terrains)\n\n"
			   "Numéro : ");
		scanf("%d", &choice);

		switch (choice) {
			case 0:
				// Sending the request
				prepare_message(&send_msg, ASK_COURTS, "");
				send_message(&socket, &send_msg, serialize_message);
				sleep(1);

				// Receiving the response
				receive_message(&socket, &received_msg, deserialize_message);
				if (received_msg.code == (char) LIST_COURTS)
					printf("Liste des terrains :\n%s\n", received_msg.data);
				else
					fprintf(stderr, "Erreur lors de la réception de la liste des terrains\n");
				break;
			default:
				// Sending the subscription message
				sprintf(data, "%d", choice);
				prepare_message(&send_msg, SUBSCRIBE, data);
				send_message(&socket, &send_msg, serialize_message);
				sleep(1);

				// Receiving the response
				printf("Attente de la validation serveur\n");
				receive_message(&socket, &received_msg, deserialize_message);

				if (received_msg.code == (char) OK) {
					subscribed = 1;
					printf("Abonnement au terrain %d réussi\n", choice);
				}
				else
					printf("Abonnement au terrain %d échoué\n", choice);
				break;
		}
	} while (!subscribed);
}

/**
 * @fn void listen_for_score(socket_t socket)
 * @brief Listens for SCORE requests and displays them
 * @param socket Server socket
 */
void listen_for_score(socket_t socket) {
	message_t received_msg;

	printf("Attente des scores...\n");
	sleep(1);

	do {
		receive_message(&socket, &received_msg, deserialize_message);

		if (received_msg.code == (char) SCORE)
			printf("Score : %s\n", received_msg.data);

		sleep(1);
	} while (received_msg.code != (char) END_MATCH);
}