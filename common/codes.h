#ifndef PANTALLA_DEPORTIVA_V2_CODES_H
#define PANTALLA_DEPORTIVA_V2_CODES_H

/**
 * @def OK
 * @brief OK code
 */
#define OK 200
/**
 * @def NOK
 * @brief NOK code
 */
#define NOK 400

/**
 * @def SCORE
 * @brief Notification code for a score update
 */
#define SCORE 2
/**
 * @def INVITE
 * @brief Request code for an invite
 */
#define INVITE 3
/**
 * @def COURT_FOUND
 * @brief Notification code when a court is found
 */
#define COURT_FOUND 4
/**
 * @def LIST_PLAYERS
 * @brief Notification code with the list of players
 */
#define LIST_PLAYERS 5
/**
 * @def INFO_PLAYER
 * @brief Notification code with the info of the player
 */
#define INFO_PLAYER 6
/**
 * @def LIST_COURTS
 * @brief Notification code with the list of courts
 */
#define LIST_COURTS 7
/**
 * @def ASK_COURTS
 * @brief Request code for getting the list of courts
 */
#define ASK_COURTS 8
/**
 * @def SUBSCRIBE
 * @brief Request code for subscribing to a court (getting the score updated)
 */
#define SUBSCRIBE 9
/**
 * @def AUTH
 * @brief Request code for authenticating
 */
#define AUTH 10
/**
 * @def ASK_PLAYERS
 * @brief Request code for getting the list of players
 */
#define ASK_PLAYERS 11
/**
 * @def PLAY_WITH
 * @brief Request code for playing with a player
 */
#define PLAY_WITH 12
/**
 * @def INCREMENT_SCORE
 * @brief Request code for incrementing the score
 */
#define INCREMENT_SCORE 13
/**
 * @def LISTEN_PORT
 * @brief Notification code for the port to listen to
 */
#define LISTEN_PORT 14
/**
 * @def END_MATCH
 * @brief Notification code for the end of a match
 */
#define END_MATCH 15

#endif //PANTALLA_DEPORTIVA_V2_CODES_H
