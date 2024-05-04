#ifndef PANTALLA_DEPORTIVA_V2_SPECTATOR_H
#define PANTALLA_DEPORTIVA_V2_SPECTATOR_H

#include <stdio.h>

#include "../socket/data.h"
#include "../serialization/serialization.h"
#include "../common/codes.h"

/**
 * @def SPECTATOR_AUTH
 * @brief This code is the one to let the server know that the client is a spectator
 */
#define SPECTATOR_AUTH 4 // 4 = Spectator

/**
 * @brief Authenticates the spectator
 * @param socket Server socket
 */
void authenticate(socket_t socket);

/**
 * @fn void select_and_subscribe(socket_t socket)
 * @brief Selects a court and subscribes to it
 * @param socket Server socket
 */
void select_and_subscribe(socket_t socket);

/**
 * @fn void listen_for_score(socket_t socket)
 * @brief Listens for SCORE requests and displays them
 * @param socket Server socket
 */
void listen_for_score(socket_t socket);

#endif //PANTALLA_DEPORTIVA_V2_SPECTATOR_H
