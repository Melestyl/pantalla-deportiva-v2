#ifndef PANTALLA_DEPORTIVA_V2_SPECTATOR_H
#define PANTALLA_DEPORTIVA_V2_SPECTATOR_H

#include <stdio.h>

#include "../socket/data.h"
#include "../serialization/serialization.h"
#include "../common/codes.h"

/**
 * @def AUTH_TYPE
 * @brief This code is the one to let the server know that the client is a spectator
 */
#define AUTH_TYPE 3 // 3 = Spectator

/**
 * @brief Authenticates the spectator
 * @param socket Server socket
 */
void authenticate(socket_t socket);

#endif //PANTALLA_DEPORTIVA_V2_SPECTATOR_H
