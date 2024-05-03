/**
 * @file court_functions.c
 * @brief Functions to manage courts
 * @date 2024-05-02
 */

#include "court_functions.h"

court_node_t* courts = NULL; // Global list of courts
pthread_mutex_t courts_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for the global list of courts

int court_id_counter = 0; // Global counter for court ids
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

	// Adding the court to the list
	add_court(court);
	printf("Court %d is available for players with %s:%d\n", court.id, court.ip, court.listen_port);

	// Responding OK to the court
	prepare_message(&send_msg, (char) OK, "");
	send_message(socket, &send_msg, serialize_message);

	//TODO: Wait for any score update
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
}