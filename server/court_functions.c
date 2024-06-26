/**
 * @file court_functions.c
 * @brief Functions to manage courts
 * @date 2024-05-02
 */

#include "court_functions.h"

court_node_t* courts = NULL; // Global list of courts
pthread_mutex_t courts_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for the global list of courts

int court_id_counter = 1; // Global counter for court ids
pthread_mutex_t court_id_counter_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for the global counter of court ids

/**
 * @fn void add_court(court_t court)
 * @brief Adds a court to the list of available courts
 * @param court: court to add (structure)
 */
void add_court(court_t court) {
	pthread_mutex_lock(&courts_mutex);

	court_node_t* new_node = (court_node_t*) malloc(sizeof(court_node_t));
	new_node->court = court;
	new_node->next = courts;
	courts = new_node;

	pthread_mutex_unlock(&courts_mutex);
}

/**
 * @fn void remove_court(int id)
 * @brief Removes a court from the list of available courts
 * @param id: court's id to remove
 */
void remove_court(int id) {
	pthread_mutex_lock(&courts_mutex);

	court_node_t* current = courts;
	court_node_t* prev = NULL;

	while (current != NULL) {
		if (current->court.id == id) {
			if (prev == NULL)
				courts = current->next;
			else
				prev->next = current->next;
			free(current);
			break;
		}
		prev = current;
		current = current->next;
	}

	pthread_mutex_unlock(&courts_mutex);
}

/**
 * @fn void new_court(void* socket)
 * @brief Thread to manage a court
 * @param socket: court's socket (for receiving the listen port and score update)
 * @param ip: court's IP
 */
void new_court(void* socket, char* ip) {
	message_t send_msg, received_msg;
	court_t court;

	// First answering OK to the court
	prepare_message(&send_msg, (char) OK, "");
	send_message(socket, &send_msg, serialize_message);

	// Waiting for a listen port
	receive_message(socket, &received_msg, deserialize_message);
	if (received_msg.code != (char) LISTEN_PORT) {
		fprintf(stderr, "Error: expected LISTEN_PORT message\n");
		return;
	}

	// Setting the IP
	strcpy(court.ip, ip);

	// Setting the listen port
	court.listen_port = atoi(received_msg.data);

	// Setting court socket
	court.socket = socket;

	// Setting court id
	pthread_mutex_lock(&court_id_counter_mutex);
	court.id = court_id_counter++;
	pthread_mutex_unlock(&court_id_counter_mutex);

	// Marking court as available
	court.available = 1;

	// Initializing the score
	strcpy(court.score, "0-0:0/0:0/0:0/0");

	// Adding the court to the list
	add_court(court);
	printf("Court %d is available for players with %s:%d\n", court.id, court.ip, court.listen_port);

	// Responding OK to the court
	prepare_message(&send_msg, (char) OK, "");
	send_message(socket, &send_msg, serialize_message);
}

/**
 * @fn court_t* get_first_available_court()
 * @brief Returns the first available court
 * @return
 */
court_t* get_first_available_court() {
	court_node_t* current = courts;

	// Searching for a court that is available
	while (current != NULL) {
		if (current->court.available)
			return &current->court;
		current = current->next;
	}

	// Returning NULL otherwise
	return NULL;
}

/**
 * @fn void listen_for_score(court_t* court)
 * @brief Listens for score and END_MATCH messages
 * @param court: court structure with all data
 */
void listen_for_score(court_t* court) {
	message_t received_msg, send_msg;

	do {
		receive_message(court->socket, &received_msg, deserialize_message);

		if (received_msg.code == (char) SCORE) {
			// Copying the score
			strcpy(court->score, received_msg.data);
			printf("Court %d: %s\n", court->id, court->score);
		}
		else if (received_msg.code == (char) END_MATCH) {
			// Removing the court from the list
			court->available = 1;
			printf("Court %d is now available\n", court->id);
		}

		// Sending OK to the court
		prepare_message(&send_msg, (char) OK, "");
		send_message(court->socket, &send_msg, serialize_message);
		sleep(1);
	} while (received_msg.code != (char) END_MATCH);
}

int court_available() {
	court_node_t* current = courts;

	while (current != NULL) {
		if (current->court.available)
			return 1;
		current = current->next;
	}

	return 0;
}

/**
 * @fn void reserve_court(player_t p1, player_t p2)
 * @brief Reserves a court for two players
 * @param p1: player 1
 * @param p2: player 2
 */
void reserve_court(player_t p1, player_t p2) {
	court_t* court = get_first_available_court();
	message_t send_msg;
	buffer_t data;

	// If no court is available, sending NOK to both players
	if (court == NULL) {
		prepare_message(&send_msg, (char) NOK, "");
		send_message(p1.socket, &send_msg, serialize_message);
		send_message(p2.socket, &send_msg, serialize_message);
		return;
	}

	// Marking the court as unavailable and setting the players
	court->available = 0;
	court->players[0] = p1;
	court->players[1] = p2;

	// Sending the court's IP and listen port to the players
	sprintf(data, "%s:%d", court->ip, court->listen_port);
	prepare_message(&send_msg, (char) COURT_FOUND, data);
	send_message(p1.socket, &send_msg, serialize_message);
	send_message(p2.socket, &send_msg, serialize_message);

	// Listening for SCORE and END_MATCH
	listen_for_score(court);
}

/**
 * @fn list_courts(socket_t socket)
 * @brief Send a list of courts to a spectator
 * @param socket: spectator's socket
 */
void list_courts(socket_t socket) {
	court_node_t* current = courts;
	message_t send_msg;
	buffer_t data, tmp;

	// Preparing the list of courts
	strcpy(data, "");
	while (current != NULL) {
		sprintf(tmp, "%d\n", current->court.id);
		strcat(data, tmp);
		current = current->next;
	}

	// Sending the list of courts
	prepare_message(&send_msg, (char) LIST_COURTS, data);
	send_message(&socket, &send_msg, serialize_message);
}

/**
 * @fn subscribe_to_court(socket_t socket, int court_id)
 * @param socket: spectator's socket
 * @param court_id: court's id
 * @return court_t*: court structure to read score afterwards, NULL if the court does not exist
 */
court_t* subscribe_to_court(socket_t socket, int court_id) {
	court_node_t* current = courts;
	message_t send_msg;

	// Searching for the court
	while (current != NULL) {
		if (current->court.id == court_id) {
			// Sending the subscription message
			prepare_message(&send_msg, (char) OK, "");
			send_message(&socket, &send_msg, serialize_message);

			return &current->court;
		}
		current = current->next;
	}

	// Sending NOK if the court does not exist
	prepare_message(&send_msg, (char) NOK, "");
	send_message(&socket, &send_msg, serialize_message);
	return NULL;

}

/**
 * @fn void watch(socket_t socket, court_t court)
 * @brief Listens for score and sends update to the spectator
 * @param spectator_socket: spectator's socket
 * @param court: court to watch for score
 */
void watch(socket_t spectator_socket, court_t* court) {
	message_t send_msg;
	court_t court_copy; // For detecting changes in the score

	// Copying the court score to detect further changes
	strcpy(court_copy.score, court->score);

	// Sending the current score
	sleep(1);
	prepare_message(&send_msg, (char) SCORE, court_copy.score);
	send_message(&spectator_socket, &send_msg, serialize_message);
	sleep(1);

	// Listening for changes in the score while the court is taken by the two players
	while (1) {
		if (strcmp(court_copy.score, court->score) != 0) {
			strcpy(court_copy.score, court->score);
			prepare_message(&send_msg, (char) SCORE, court_copy.score);
			if (send_message(&spectator_socket, &send_msg, serialize_message) == -1)
				break;
			sleep(1);
		}
	}
}

/**
 * @fn spectator_function(socket_t socket)
 * @brief Function to manage a spectator
 * @param socket: spectator's socket
 */
void spectator_function(socket_t* socket) {
	message_t send_msg, received_msg;
	int subscribed = 0;
	court_t* court;

	sleep(1);

	// Answering OK
	prepare_message(&send_msg, (char) OK, "");
	send_message(socket, &send_msg, serialize_message);
	sleep(1);

	do {
		receive_message(socket, &received_msg, deserialize_message);

		switch (received_msg.code) {
			case ASK_COURTS:
				printf("Spectator is asking for the list of courts\n");
				list_courts(*socket);
				break;
			case SUBSCRIBE:
				printf("Spectator wants to subscribe to court %s\n", received_msg.data);
				court = subscribe_to_court(*socket, atoi(received_msg.data));
				if (court != NULL) {
					subscribed = 1;
					printf("Spectator has subscribed to court %d\n", court->id);
					watch(*socket, court);
				}
				break;
		}
	} while (!subscribed);
}