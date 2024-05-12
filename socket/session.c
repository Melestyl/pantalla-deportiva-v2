/**
 * @file session.c
 * @brief Session Layer Implementation
 * @date 2024-04-24
 * @version 1.0
 * @authors
 * 	- TELLIER--CALOONE Tom
 * 	- DELANNOY Anaël
 */

#include "session.h"

/**
 * @fn void addr2struct(struct sockaddr_in *addr, char *ip_address, short port)
 * @brief Fill a sockaddr_in structure with an IP address and a port
 * @param addr: pointer to the sockaddr_in structure to fill
 * @param ip_address: IP address
 * @param port: port
 */
void addr2struct(struct sockaddr_in *addr, char *ip_address, short port) {
	// Fulfilling the structure
	addr->sin_family = PF_INET;
	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = inet_addr(ip_address);

	// Filling the rest with 0
	memset(addr->sin_zero, 0, 8);
}

/**
 * @fn socket_t create_socket(int mode)
 * @brief Create a socket
 * @param mode: socket mode (SOCK_STREAM or SOCK_DGRAM)
 * @return the created socket
 */
socket_t create_socket(int mode){
	socket_t sock;

	// Checking the socket mode
	if (mode != SOCK_STREAM && mode != SOCK_DGRAM){
		fprintf(stderr, "Unknown socket mode (must be either SOCK_STREAM or SOCK_DGRAM\n");
		exit(1);
	}

	// Creating the socket
	sock.mode = mode;
	CHECK(sock.file_descriptor = socket(PF_INET, mode, 0), "Can't create socket");

	return sock;
}

/**
 * @fn socket_t create_socket_adr(int mode, char *ip_address, short port)
 * @brief Create a socket with an address
 * @param mode: socket mode (SOCK_STREAM or SOCK_DGRAM)
 * @param ip_address: IP address
 * @param port: port
 * @return socket with the specified address
 */
socket_t create_addressed_socket(int mode, char *ip_address, short port){
	socket_t sock;
	socklen_t len;

	// Creating the socket
	sock = create_socket(mode);

	// Filling the structure
	addr2struct(&sock.local_address, ip_address, port);

	// Assigning the address to the socket
	CHECK(bind(sock.file_descriptor, (struct sockaddr *)&sock.local_address, sizeof(sock.local_address)), "Can't bind socket");

	// Retrieving the local address
	len = sizeof(sock.local_address);
	CHECK(getsockname(sock.file_descriptor, (struct sockaddr *)&sock.local_address, &len), "Can't get local address");

	return sock;
}

/**
 * @fn socket_t create_listen_socket(char *ip_address, short port)
 * @brief Create a listening socket with the specified address
 * @param ip_address: server IP address to listen to
 * @param port: server TCP port to listen to
 * @return socket created with the specified address and in a listening state
 */
socket_t create_listen_socket(char *ip_address, short port){
	socket_t sock;

	// Creating the socket
	sock = create_addressed_socket(SOCK_STREAM, ip_address, port);

	// Listening, with a maximum of 5 clients
	CHECK(listen(sock.file_descriptor, 5), "Can't listen on socket");

	return sock;
}

/**
 * @fn socket_t accept_client(const socket_t listen_socket)
 * @brief Accept a client connection
 * @param listen_socket: listening socket
 * @return the socket created for the client
 */
socket_t accept_client(const socket_t listen_socket){
	socket_t dialog_socket;
	socklen_t addr_len;

	addr_len = sizeof(struct sockaddr_in);

	// Accepting the client
	CHECK(dialog_socket.file_descriptor = accept(
			listen_socket.file_descriptor,
			(struct sockaddr *)&listen_socket.remote_address,
					&addr_len),
		  "Can't accept");

	// Filling the structure
	dialog_socket.mode = listen_socket.mode;
	dialog_socket.remote_address = listen_socket.remote_address;
	dialog_socket.local_address = listen_socket.local_address;

	return dialog_socket;
}

/**
 * @fn socket_t connect_to(char *ip_address, short port)
 * @brief Connect to another socket (client or server)
 * @param ip_address: remote IP address
 * @param port: remote port
 * @return the connected socket
 */
socket_t connect_to(char *ip_address, short port){
	socket_t sock;

	// Creating the socket
	sock = create_socket(SOCK_STREAM);

	// Fulfilling the structure of the remote address
	addr2struct(&sock.remote_address, ip_address, port);

	// Connecting to the server
	CHECK(connect(sock.file_descriptor, (struct sockaddr *)&sock.remote_address, sizeof(sock.remote_address)), "Can't connect to remote socket");

	// Retrieving the local address
	socklen_t len = sizeof(sock.local_address);
	CHECK(getsockname(sock.file_descriptor, (struct sockaddr *)&sock.local_address, &len), "Can't get local address");

	return sock;
}