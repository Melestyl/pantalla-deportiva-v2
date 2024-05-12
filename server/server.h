#ifndef PANTALLA_DEPORTIVA_V2_SERVER_H
#define PANTALLA_DEPORTIVA_V2_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#include "../socket/data.h"
#include "../serialization/serialization.h"
#include "../common/codes.h"

/**
 * @fn void listen_thread(void* socket)
 * @brief Thread to listen to a client.
 * @param socket: client socket created after an accept
 */
void listen_thread(void* socket);

/**
 * @fn void sigint_handler(int signum)
 * @brief Signal handler for SIGINT, closing the socket properly
 * @param signum: unused
 */
void sigint_handler(int signum);

#endif //PANTALLA_DEPORTIVA_V2_SERVER_H
