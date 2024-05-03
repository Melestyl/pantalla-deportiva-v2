#ifndef PANTALLA_DEPORTIVA_V2_PLAYER_H
#define PANTALLA_DEPORTIVA_V2_PLAYER_H

#include <stdio.h>

#include "../socket/data.h"
#include "../serialization/serialization.h"
#include "../common/codes.h"

/**
 * @def HOST_AUTH
 * @brief This code is the one to let the server know that the client is a player (inviting)
 */
#define HOST_AUTH 1 // Player who invites

/**
 * @def INVITED_AUTH
 * @brief This code is the one to let the server know that the client is a player (invited)
 */
#define INVITED_AUTH 2 // Player who is invited

/**
 * @brief Authenticates the player
 * @param socket: Server socket
 * @param type: Whether the player is inviting or invited
 * @param first_name: Player's first name
 * @param last_name: Player's last name
 */
void authenticate(socket_t socket, char type, char* first_name, char* last_name);

/**
 * @fn void wait_for_partner(socket_t socket)
 * @brief Function for the player waiting for an invitation
 * @param socket: Server socket
 */
void wait_for_partner(socket_t socket);

/**
 * @fn void invited_player(socket_t socket)
 * @brief Function for the invited player
 * @param socket: Server socket
 */
void invited_player(socket_t socket);

#endif //PANTALLA_DEPORTIVA_V2_PLAYER_H
