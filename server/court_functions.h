#ifndef PANTALLA_DEPORTIVA_V2_COURT_FUNCTIONS_H
#define PANTALLA_DEPORTIVA_V2_COURT_FUNCTIONS_H

#include "server.h"
#include "player_functions.h"

/**
 * @struct court
 * @brief Structure to keep infos about a court
 * @var id: court's id
 * @var socket: court's socket used to receive the score
 * @var listen_port: port to send players on
 * @var players: players in the court (for printing names only)
 * @var available: 1 if the court is available, 0 otherwise
 */
struct court {
	int id;
	socket_t* socket;
	char ip[16];
	int listen_port;
	player_t players[2];
	char available;
	//TODO: Score structure
};

/**
 * @typedef court_t
 * @brief Typedef for court structure
 */
typedef struct court court_t;

/**
 * @struct court_node
 * @brief Structure of a list of courts
 * @var court: court
 * @var next: next court in the list
 */
struct court_node {
	court_t court;
	struct court_node* next;
};

/**
 * @typedef court_node_t
 * @brief Typedef for court_node structure
 */
typedef struct court_node court_node_t;

/**
 * @fn void new_court(void* socket)
 * @brief Thread to manage a court
 * @param socket: court's socket (for receiving the listen port and score update)
 * @param ip: court's IP
 */
void new_court(void* socket, char* ip);

/**
 * @fn void reserve_court(player_t p1, player_t p2)
 * @brief Reserves a court for two players
 * @param p1: player 1
 * @param p2: player 2
 */
void reserve_court(player_t p1, player_t p2);

/**
 * @fn list_courts(socket_t socket)
 * @brief Send a list of courts to a spectator
 * @param socket: spectator's socket
 */
void list_courts(socket_t socket);

/**
 * @fn subscribe_to_court(socket_t socket, int court_id)
 * @param socket: spectator's socket
 * @param court_id: court's id
 * @return court_t*: court structure to read score afterwards, NULL if the court does not exist
 */
court_t* subscribe_to_court(socket_t socket, int court_id);

/**
 * @fn void listen_and_update(socket_t socket, court_t court)
 * @brief Listens for score and sends update to the spectator
 * @param spectator_socket: spectator's socket
 * @param court: court to watch for score
 */
void listen_and_update(socket_t* spectator_socket, court_t* court);

/**
 * @fn spectator_function(socket_t socket)
 * @brief Function to manage a spectator
 * @param socket: spectator's socket
 */
void spectator_function(socket_t* socket);

#endif //PANTALLA_DEPORTIVA_V2_COURT_FUNCTIONS_H
