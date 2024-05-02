#ifndef PANTALLA_DEPORTIVA_V2_COURT_H
#define PANTALLA_DEPORTIVA_V2_COURT_H

#include <stdio.h>

#include "../socket/data.h"
#include "../serialization/serialization.h"
#include "../common/codes.h"

#define LOVE 0
#define FIFTEEN 1
#define THIRTY 2
#define FORTY 3
#define ADVANTAGE 4

struct score {
	int player1;
	int player2;
	int player1_games[3];
	int player2_games[3];
	int current_set;
};

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
 * @brief Thread for the player
 * @param player_socket: Player socket
 */
void player_thread(void* player_socket);

#endif //PANTALLA_DEPORTIVA_V2_COURT_H
