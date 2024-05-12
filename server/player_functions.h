#ifndef PANTALLA_DEPORTIVA_V2_PLAYER_FUNCTIONS_H
#define PANTALLA_DEPORTIVA_V2_PLAYER_FUNCTIONS_H

#include "server.h"

/**
 * @struct player
 * @brief Structure to keep infos about a player
 * @var socket: player's socket to send the invitation and then the court
 * @var first_name: player's first name
 * @var last_name: player's last name
 */
struct player {
	socket_t* socket;
	int id;
	buffer_t first_name;
	buffer_t last_name;
};

/**
 * @typedef player_t
 * @brief Typedef for player structure
 */
typedef struct player player_t;

/**
 * @struct player_node
 * @brief Structure of a list of players
 * @var player: player
 * @var next: next player in the list
 */
struct player_node {
	player_t player;
	struct player_node* next;
};

/**
 * @typedef player_node_t
 * @brief Typedef for player_node structure
 */
typedef struct player_node player_node_t;

/**
 * @fn void invited_player(socket_t* client_socket, buffer_t ip, int port)
 * @brief Function to handle an invited player
 * @param client_socket: socket of the current player
 * @param data: data received from the client when authenticating
 */
void invited_player(socket_t* client_socket, char* data);

/**
 * @fn player_t* invite_player(int id)
 * @brief Function to invite a player
 * @param host_socket: socket of the host
 * @param id: id of the player to invite
 * @param host: host player structure
 * @param partner_player: player structure filled if the player has accepted the invitation
 * @return int: 1 if the player has accepted the invitation (or exists), 0 otherwise
 */
int invite_player(socket_t host_socket, int id, player_t* host, player_t* partner_player);

/**
 * @fn void list_players(socket_t host_socket)
 * @brief Send to host_socket the list of available players
 * @param host_socket: socket of the host
 */
void list_players(socket_t* host_socket);

/**
 * @fn void host_player(socket_t client_socket, buffer_t ip, int port)
 * @brief Function to handle a player who invites
 * @param client_socket: socket of the current player
 * @param data: data received from the client when authenticating
 */
void host_player(socket_t* client_socket, char* data);

#endif //PANTALLA_DEPORTIVA_V2_PLAYER_FUNCTIONS_H
