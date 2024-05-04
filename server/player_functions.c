/**
 * @file player_functions.c
 * @brief Functions to manage players
 * @date 2024-05-02
 */

#include "player_functions.h"

player_node_t* players = NULL; // Global list of players
pthread_mutex_t players_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for the global list of players

int player_id_counter = 1; // Global counter for player ids
pthread_mutex_t id_counter_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for the global counter of player ids

/**
 * @fn void add_player(player_t player)
 * @brief Adds a player to the list of available players
 * @param player: player to add (structure)
 */
void add_player(player_t player) {
	pthread_mutex_lock(&players_mutex);

	player_node_t* new_node = (player_node_t*) malloc(sizeof(player_node_t));
	new_node->player = player;
	new_node->next = players;
	players = new_node;

	pthread_mutex_unlock(&players_mutex);
}

/**
 * @fn void remove_player(int id)
 * @brief Removes a player from the list of available players (if they are invited)
 * @param id: player's id
 */
void remove_player(int id) {
	pthread_mutex_lock(&players_mutex);

	player_node_t* current = players;
	player_node_t* prev = NULL;

	while (current != NULL) {
		if (current->player.id == id) {
			if (prev == NULL)
				players = current->next;
			else
				prev->next = current->next;
			free(current);
			break;
		}
		prev = current;
		current = current->next;
	}

	pthread_mutex_unlock(&players_mutex);
}

/**
 * @fn void invited_player(socket_t* client_socket, buffer_t ip, int port)
 * @brief Function to handle an invited player
 * @param client_socket: socket of the current player
 * @param data: data received from the client when authenticating
 */
void invited_player(socket_t* client_socket, char* data) {
	char *save_ptr, *token;
	message_t send_msg;
	player_t player;
	buffer_t id_str;

	// Skipping the auth code
	data = strchr(data, ':') + 1;

	// Retrieving the player's last name
	token = strtok_r(data, ":", &save_ptr);
	if (token == NULL) {
		prepare_message(&send_msg, (char) NOK, "");
		send_message(client_socket, &send_msg, serialize_message);
		close(client_socket->file_descriptor);
		return;
	}
	strcpy(player.last_name, token);

	// Retrieving the player's first name
	token = strtok_r(NULL, ":", &save_ptr);
	if (token == NULL) {
		prepare_message(&send_msg, (char) NOK, "");
		send_message(client_socket, &send_msg, serialize_message);
		close(client_socket->file_descriptor);
		return;
	}
	strcpy(player.first_name, token);

	// Setting the player's socket
	player.socket = client_socket;

	// Creating the player's id
	pthread_mutex_lock(&id_counter_mutex);
	player.id = player_id_counter++;
	pthread_mutex_unlock(&id_counter_mutex);

	// Adding client to the list of available players
	add_player(player);
	printf("'%s %s' (%d) has been added to the list of available players\n", player.first_name, player.last_name, player.id);

	// Answer OK to the client
	prepare_message(&send_msg, (char) OK, "");
	send_message(client_socket, &send_msg, serialize_message);

	sleep(1); // Mandatory sleep to prevent an issue when sending two messages in a row

	// Giving the player its id
	sprintf(id_str, "%d", player.id);
	prepare_message(&send_msg, (char) INFO_PLAYER, id_str);
	send_message(client_socket, &send_msg, serialize_message);
}