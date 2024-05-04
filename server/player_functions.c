/**
 * @file player_functions.c
 * @brief Functions to manage players
 * @date 2024-05-02
 */

#include "player_functions.h"
#include "court_functions.h"

player_node_t* players = NULL; // Global list of players
pthread_mutex_t players_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for the global list of players

int player_id_counter = 0; // Global counter for player ids
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
	printf("'%s %s' (%d) has been added to the list of available players\n",
		   player.first_name, player.last_name, player.id);

	// Answer OK to the client
	prepare_message(&send_msg, (char) OK, "");
	send_message(client_socket, &send_msg, serialize_message);

	sleep(1); // Mandatory sleep to prevent an issue when sending two messages in a row

	// Giving the player its id
	sprintf(id_str, "%d", player.id);
	prepare_message(&send_msg, (char) INFO_PLAYER, id_str);
	send_message(client_socket, &send_msg, serialize_message);
}

/**
 * @fn player_t* invite_player(int id)
 * @brief Function to invite a player
 * @param host_socket: socket of the host
 * @param id: id of the player to invite
 * @param host: host player structure
 * @param partner_player: player structure filled if the player has accepted the invitation
 * @return int: 1 if the player has accepted the invitation (or exists), 0 otherwise
 */
int invite_player(socket_t host_socket, int id, player_t* host, player_t* partner_player) {
	player_node_t* current = players;
	message_t send_msg, received_msg;
	buffer_t data;

	// Searching for the player to invite
	while (current != NULL) {
		if (current->player.id == id) {
			// Sending the invitation
			sprintf(data, "%s:%s", host->last_name, host->first_name);
			prepare_message(&send_msg, (char) INVITE, data);
			send_message(current->player.socket, &send_msg, serialize_message);
			break;
		}
		current = current->next;
	}

	if (current == NULL) {
		prepare_message(&send_msg, (char) NOK, "");
		send_message(&host_socket, &send_msg, serialize_message);
		return 0;
	}

	// Waiting for its validation
	receive_message(current->player.socket, &received_msg, deserialize_message);
	if (received_msg.code != (char) OK) {
		prepare_message(&send_msg, (char) NOK, "");
		send_message(&host_socket, &send_msg, serialize_message);
		return 0;
	}

	// Copying the player structure
	partner_player->id = current->player.id;
	strcpy(partner_player->first_name, current->player.first_name);
	strcpy(partner_player->last_name, current->player.last_name);
	partner_player->socket = current->player.socket;

	// Removing the player from the list of available players
	remove_player(id);

	// Answering OK to the host
	prepare_message(&send_msg, (char) OK, "");
	send_message(&host_socket, &send_msg, serialize_message);

	return 1;
}

/**
 * @fn void list_players(socket_t host_socket)
 * @brief Send to host_socket the list of available players
 * @param host_socket: socket of the host
 */
void list_players(socket_t* host_socket) {
	player_node_t* current = players;
	message_t send_msg;
	buffer_t tmp, data;

	// Filling tmp with the players' data, and concatenating into data
	while (current != NULL) {
		sprintf(tmp, "%d:%s:%s:", current->player.id, current->player.last_name, current->player.first_name);
		strcat(data, tmp);
		current = current->next;
	}
	// An example of the formatted data is: "1:DOE:John:2:SMITH:Jane:3:DELANNOY:Anael"

	// Deleting last :
	data[strlen(data) - 1] = '\0';

	// Sending the list of players
	prepare_message(&send_msg, (char) LIST_PLAYERS, data);
	send_message(host_socket, &send_msg, serialize_message);
}

/**
 * @fn void host_player(socket_t client_socket, buffer_t ip, int port)
 * @brief Function to handle a player who invites
 * @param client_socket: socket of the current player
 * @param data: data received from the client when authenticating
 */
void host_player(socket_t* client_socket, char* data) {
	char *save_ptr, *token;
	message_t send_msg, received_msg;
	player_t host, partner_player;
	int partner_found = 0;

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
	strcpy(host.last_name, token);

	// Retrieving the player's first name
	token = strtok_r(NULL, ":", &save_ptr);
	if (token == NULL) {
		prepare_message(&send_msg, (char) NOK, "");
		send_message(client_socket, &send_msg, serialize_message);
		close(client_socket->file_descriptor);
		return;
	}
	strcpy(host.first_name, token);

	// Setting the player's socket
	host.socket = client_socket;

	// Creating the player's id
	pthread_mutex_lock(&id_counter_mutex);
	host.id = player_id_counter++;
	pthread_mutex_unlock(&id_counter_mutex);

	// Answer OK to the client
	prepare_message(&send_msg, (char) OK, "");
	send_message(client_socket, &send_msg, serialize_message);

	// Receiving and handling its requests
	do {
		receive_message(client_socket, &received_msg, deserialize_message);

		switch (received_msg.code) {
			case PLAY_WITH:
				// Inviting a player
				if (invite_player(*client_socket, atoi(received_msg.data), &host, &partner_player)) {
					partner_found = 1;
					reserve_court(host, partner_player);
				}
				break;

			case ASK_PLAYERS:
				// Sending a list of available players
				list_players(client_socket);
				break;

			default:
				prepare_message(&send_msg, (char) NOK, "");
				send_message(client_socket, &send_msg, serialize_message);
				break;
		}
	} while (received_msg.code != (char) PLAY_WITH && partner_found == 0);
}