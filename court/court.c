/**
 * @file court.c
 * @brief Court module
 * @date 2024-05-01
 */

#include "court.h"

socket_t listen_socket, server_socket; // Declared globally to be accessed from functions

score_t score; // Global score
pthread_mutex_t score_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for the score

int main(int argc, char** argv) {
	socket_t player1, player2;
	pthread_t p1_thread, p2_thread;
	message_t send_msg, received_msg;
	player_data_t player1_data, player2_data; // Structure used to pass two arguments to the player_thread function

	if (argc < 3) {
		fprintf(stderr, "Usage: %s <ServerIP> <ServerPort>\n", argv[0]);
		return 1;
	}

	// Connecting to the server
	server_socket = connect_to(argv[1], atoi(argv[2]));

	// Setting up signal handler to close the socket properly
	signal(SIGINT, sigint_handler);

	// Authenticating
	authenticate(server_socket);

	// Opening listen socket
	listen_socket = create_listen_socket("0.0.0.0", 0);

	// Sending listen port to server
	send_listen_port(
			server_socket,
			ntohs(((struct sockaddr_in*)&listen_socket.local_address)->sin_port)
	);

	while (1) {
		// Waiting for incoming connections (2 players)
		player1 = accept_client(listen_socket);
		printf("Player 1 connected\n");
		player2 = accept_client(listen_socket);
		printf("Player 2 connected\n");

		// Initializing score
		init_score();

		// Creating player data for both of them
		player1_data.socket = &player1;
		player1_data.player_number = 1;
		player2_data.socket = &player2;
		player2_data.player_number = 2;

		// Creating threads for the players
		pthread_create(&p1_thread, NULL, (void *) player_thread, (void *) &player1_data);
		pthread_create(&p2_thread, NULL, (void *) player_thread, (void *) &player2_data);

		// Waiting for the game to finish
		while (!is_match_finished())
			sleep(1);

		// Send an END_MATCH message to the server
		send_end_match(server_socket);
	}
}

/**
 * @brief Authenticates the spectator
 * @param socket: Server socket
 */
void authenticate(socket_t socket) {
	message_t message;
	char data[2];

	sprintf(data, "%d", COURT_AUTH); // Converting SPECTATOR_AUTH to a string

	// Preparing the authentication message
	prepare_message(&message, AUTH, data);

	// Sending the message
	send_message(&socket, &message, serialize_message);

	// Waiting for the OK response
	receive_message(&socket, &message, deserialize_message);

	if (message.code == (char) OK)
		printf("Authenticated successfully\n");
	else
		printf("Authentication failed\n");
}


/**
 * @brief Sends the listen port to the server
 * @param socket: Server socket
 * @param port: Listen port
 */
void send_listen_port(socket_t socket, int port) {
	message_t message;
	char data[6];

	sprintf(data, "%d", port); // Converting port to a string

	// Preparing the message
	prepare_message(&message, LISTEN_PORT, data);

	// Sending the message
	send_message(&socket, &message, serialize_message);
}

/**
 * @fn void init_score(score_t *score)
 * @brief Initializes the score structure
 */
void init_score() {
	pthread_mutex_lock(&score_mutex);

	score.player1 = LOVE;
	score.player2 = LOVE;
	score.player1_games[0] = 0;
	score.player1_games[1] = 0;
	score.player1_games[2] = 0;
	score.player2_games[0] = 0;
	score.player2_games[1] = 0;
	score.player2_games[2] = 0;
	score.player1_sets = 0;
	score.player2_sets = 0;
	score.current_set = 0;

	pthread_mutex_unlock(&score_mutex);
}

/**
 * @fn int is_match_finished()
 * @brief Checks if the match is finished by looking at the sets won by each player
 * @return 1 if the match is finished, 0 otherwise
 */
int is_match_finished() {
	return (score.player1_sets == 2 || score.player2_sets == 2);
}

/**
 * @fn void increment_score(int player)
 * @brief Increments the score of a player
 * @param player: Player to increment the score (1 or 2)
 */
void increment_score(int player) {
	int *player_score, *opponent_score, *player_games, *opponent_games, *player_sets;

	pthread_mutex_lock(&score_mutex);

	if (player == 1) {
		player_score = &score.player1;
		opponent_score = &score.player2;
		player_games = score.player1_games;
		opponent_games = score.player2_games;
		player_sets = &score.player1_sets;
	}
	else {
		player_score = &score.player2;
		opponent_score = &score.player1;
		player_games = score.player2_games;
		opponent_games = score.player1_games;
		player_sets = &score.player2_sets;
	}

	switch (*player_score) {
		case LOVE:
		case FIFTEEN:
		case THIRTY:
			(*player_score)++;
			break;
		case FORTY:
			if (*opponent_score == FORTY)
				*player_score = ADVANTAGE;
			else if (*opponent_score == ADVANTAGE)
				*opponent_score = FORTY;
			else {
				player_games[score.current_set]++;
				*player_score = LOVE;
				*opponent_score = LOVE;
			}
			break;
		case ADVANTAGE:
			player_games[score.current_set]++;
			*player_score = LOVE;
			*opponent_score = LOVE;
			break;
	}

	// If the set is finished, increment the current set (and increment the winner's set count)
	if ((player_games[score.current_set] == 6 && opponent_games[score.current_set] <= 4)
	|| (player_games[score.current_set] == 7)) {
		score.current_set++;
		(*player_sets)++;
	}

	printf("Score incremented by player %d\n", player);
	printf("New score : %d/%d:%d/%d:%d/%d:%d/%d\n",
			score.player1, score.player2,
			score.player1_games[0], score.player2_games[0],
			score.player1_games[1], score.player2_games[1],
			score.player1_games[2], score.player2_games[2]);

	pthread_mutex_unlock(&score_mutex);
}

/**
 * @fn void send_score_to_server()
 * @brief Sends an update message to the server with the current score
 */
void send_score_to_server() {
	message_t send_msg;
	buffer_t data;
	char formatted_p1_score[5], formatted_p2_score[5];

	pthread_mutex_lock(&score_mutex);

	switch (score.player1) {
		case LOVE:
			strcpy(formatted_p1_score, "0");
			break;
		case FIFTEEN:
			strcpy(formatted_p1_score, "15");
			break;
		case THIRTY:
			strcpy(formatted_p1_score, "30");
			break;
		case FORTY:
			strcpy(formatted_p1_score, "40");
			break;
		case ADVANTAGE:
			strcpy(formatted_p1_score, "ADV");
			break;
	}
	switch (score.player2) {
		case LOVE:
			strcpy(formatted_p2_score, "0");
			break;
		case FIFTEEN:
			strcpy(formatted_p2_score, "15");
			break;
		case THIRTY:
			strcpy(formatted_p2_score, "30");
			break;
		case FORTY:
			strcpy(formatted_p2_score, "40");
			break;
		case ADVANTAGE:
			strcpy(formatted_p2_score, "ADV");
			break;
	}

	// Formatting data
	sprintf(data, "%s/%s:%d/%d:%d/%d:%d/%d",
			formatted_p1_score, formatted_p2_score,
			score.player1_games[0], score.player2_games[0],
			score.player1_games[1], score.player2_games[1],
			score.player1_games[2], score.player2_games[2]
	);
	// Formatted examples:
	// 30/30:4/2:0/0:0/0
	// 40/15:6/1:4/2:0/0

	pthread_mutex_unlock(&score_mutex);

	// Sending message
	prepare_message(&send_msg, SCORE, data);
	send_message(&server_socket, &send_msg, serialize_message);

	// Waiting for OK
	receive_message(&server_socket, &send_msg, deserialize_message);
	if (send_msg.code == (char) OK)
		printf("Score sent to the server successfully.\n");
	else
		printf("Server has answered NOK when updating the score.\n");
}

/**
 * @fn void player_thread(void* player_data)
 * @brief Thread for the player
 * @param player_data: Structure containing the player's socket and number
 */
void player_thread(void* player_data) {
	player_data_t* player = ((player_data_t*) player_data);
	message_t received_msg, send_msg;

	// Waiting for any score-increment update
	while (1) {
		printf("Waiting for a message from player %d...\n", player->player_number);
		receive_message(player->socket, &received_msg, deserialize_message);

		if (received_msg.code == (char) INCREMENT_SCORE) {
			if (!is_match_finished()) {
				// Incrementing the score
				increment_score(player->player_number);

				// Sending the updated score to the server
				send_score_to_server();

				// Answering OK to the player
				prepare_message(&send_msg, (char) OK, "");
				send_message(player->socket, &send_msg, serialize_message);
			}
			else {
				// Answering the player the match is finished
				prepare_message(&send_msg, (char) END_MATCH, "");
				send_message(player->socket, &send_msg, serialize_message);
				break;
			}
		}
		else
			fprintf(stderr, "Received unknown message from player %d: %d\n", player->player_number, received_msg.code);
	}

	printf("Match ended!\n");
}

/**
 * @fn void send_end_match(socket_t socket)
 * @brief Sends an END_MATCH message to the server
 * @param socket: Server socket
 */
void send_end_match(socket_t socket) {
	message_t message;

	// Sending END_MATCH
	prepare_message(&message, END_MATCH, "");
	send_message(&socket, &message, serialize_message);

	// Waiting for OK
	receive_message(&socket, &message, deserialize_message);
	if (message.code == (char) OK)
		printf("Match ended successfully (server is OK).\n");
	else
		printf("Match ended unsuccessfully (server is NOK). Please reload the court.\n");
}

/**
 * @fn void sigint_handler(int signum)
 * @brief Signal handler for SIGINT, closing the socket properly
 * @param signum: unused
 */
void sigint_handler(int signum) {
	close(listen_socket.file_descriptor);
	close(server_socket.file_descriptor);
	printf("\nCourt closed.\n");
	exit(0);
}