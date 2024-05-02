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

#endif //PANTALLA_DEPORTIVA_V2_PLAYER_H
