#ifndef PANTALLA_DEPORTIVA_V2_SERVER_H
#define PANTALLA_DEPORTIVA_V2_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../socket/data.h"
#include "../serialization/serialization.h"

#define OK 200
#define NOK 400

#define SCORE 2
#define INVITE 3
#define COURT_FOUND 4
#define LIST_PLAYERS 5
#define INFO_PLAYERS 6
#define LIST_COURTS 7
#define ASK_COURTS 8
#define SUBSCRIBE 9
#define AUTH 10
#define ASK_PLAYERS 11
#define PLAY_WITH 12
#define INCREMENT_SCORE 13
#define LISTEN_PORT 14
#define END_MATCH 15

#endif //PANTALLA_DEPORTIVA_V2_SERVER_H
