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

#endif //PANTALLA_DEPORTIVA_V2_COURT_FUNCTIONS_H
