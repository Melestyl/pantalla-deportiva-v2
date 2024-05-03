#ifndef PANTALLA_DEPORTIVA_V2_COURT_H
#define PANTALLA_DEPORTIVA_V2_COURT_H

#include <stdio.h>
#include <pthread.h>

#include "../socket/data.h"
#include "../serialization/serialization.h"
#include "../common/codes.h"

/**
 * @def LOVE
 * @brief Score of 0 points
 */
#define LOVE 0
/**
 * @def FIFTEEN
 * @brief Score of 15 point
 */
#define FIFTEEN 1
/**
 * @def THIRTY
 * @brief Score of 30 points
 */
#define THIRTY 2
/**
 * @def FORTY
 * @brief Score of 40 points
 */
#define FORTY 3
/**
 * @def ADVANTAGE
 * @brief When a player has the advantage (one point after 40)
 */
#define ADVANTAGE 4

/**
 * @struct score
 * @brief Structure to store the score of the match
 * @var player1: Score of player 1
 * @var player2: Score of player 2
 * @var player1_games: Array with the games won by player 1 in each set
 * @var player2_games: Array with the games won by player 2 in each set
 * @var player1_sets: Number of sets won by player 1
 * @var player2_sets: Number of sets won by player 2
 * @var current_set: Current set being played (0, 1 or 2)
 */
struct score {
	int player1;
	int player2;
	int player1_games[3];
	int player2_games[3];
	int player1_sets;
	int player2_sets;
	int current_set;
};

/**
 * @typedef score_t
 * @brief Typedef for the score structure
 */
typedef struct score score_t;

/**
 * @struct player_data
 * @brief Structure to store the player's socket and number
 * @var socket: Player's socket
 * @var player_number: Player's number (1 or 2)
 */
struct player_data {
	socket_t* socket;
	int player_number;
};

/**
 * @typedef player_data_t
 * @brief Typedef for the player_data structure
 */
typedef struct player_data player_data_t;

/**
 * @def COURT_AUTH
 * @brief This code is the one to let the server know that the client is a court
 */
#define COURT_AUTH 3 // Court code for authentication

/**
 * @brief Authenticates the spectator
 * @param socket: Server socket
 */
void authenticate(socket_t socket);

/**
 * @brief Sends the listen port to the server
 * @param socket: Server socket
 * @param port: Listen port
 */
void send_listen_port(socket_t socket, int port);

/**
 * @fn void init_score(score_t *score)
 * @brief Initializes the score structure
 */
void init_score();

/**
 * @fn int is_match_finished()
 * @brief Checks if the match is finished by looking at the sets won by each player
 * @return 1 if the match is finished, 0 otherwise
 */
int is_match_finished();

/**
 * @fn void increment_score(int player)
 * @brief Increments the score of a player
 * @param player: Player to increment the score (1 or 2)
 */
void increment_score(int player);

/**
 * @fn void player_thread(void* player_data)
 * @brief Thread for the player
 * @param player_data: Structure containing the player's socket and number
 */
void player_thread(void* player_data);

/**
 * @fn void send_end_match(socket_t socket)
 * @brief Sends an END_MATCH message to the server
 * @param socket: Server socket
 */
void send_end_match(socket_t socket);

#endif //PANTALLA_DEPORTIVA_V2_COURT_H
