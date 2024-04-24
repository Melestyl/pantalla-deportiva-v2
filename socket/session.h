/**
 * @file session.h
 * @brief Session Layer specification
 * @date 2024-04-24
 * @version 1.0
 * @authors
 * 	- TELLIER--CALOONE Tom
 * 	- DELANNOY Anaël
 */

#ifndef SESSION_H
#define SESSION_H
/*
*****************************************************************************************
 *			S P E C I F I C   I N C L U D E S
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/*
*****************************************************************************************
 *			C O N S T A N T S   D E F I N I T I O N
 */
/**
 *	@def		CHECK(sts, msg)
 *	@brief 		Macro-function that checks if sts is equal to -1 (error case: sts==-1)
 *				In case of error, the appropriate message is displayed and the execution is stopped
 */
#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}
/**
 *	@def		PAUSE(msg)
 *	@brief		Macro-function that displays a message and waits for the user to press Enter
 */
#define PAUSE(msg)	printf("%s \n[Press Enter to continue...]", msg); getchar();
/*
*****************************************************************************************
 *			D A T A   S T R U C T U R E S
 */
/**
 *	@struct		socket
 *	@brief		Socket data structure
 *	@note 		This type is composed of the socket file_descriptor, the mode (connected/not)
 *				and the application addresses (local/remote)
 *	@var		file_descriptor: socket file descriptor
 *	@var		mode: connected mode (STREAM/DGRAM)
 *	@var		local_address: local socket address
 *	@var		remote_address: remote socket address
 */
struct socket {
	int file_descriptor;
	int mode;
	struct sockaddr_in local_address;
	struct sockaddr_in remote_address;
};
/**
 *	@typedef	socket_t
 *	@brief		socket_t type definition
 */
typedef struct socket socket_t; 
/*
*****************************************************************************************
 *			F U N C T I O N   P R O T O T Y P E S
 */

/**
 * @fn void addr2struct(struct sockaddr_in *addr, char *ip_address, short port)
 * @brief Fill a sockaddr_in structure with an IP address and a port
 * @param addr: pointer to the sockaddr_in structure to fill
 * @param ip_address: IP address
 * @param port: port
 */
void addr2struct(struct sockaddr_in *addr, char *ip_address, short port);

/**
 * @fn socket_t create_socket(int mode)
 * @brief Create a socket
 * @param mode: socket mode (SOCK_STREAM or SOCK_DGRAM)
 * @return the created socket
 */
socket_t create_socket(int mode);

/**
 * @fn socket_t create_socket_adr(int mode, char *ip_address, short port)
 * @brief Create a socket with an address
 * @param mode: socket mode (SOCK_STREAM or SOCK_DGRAM)
 * @param ip_address: IP address
 * @param port: port
 * @return socket with the specified address
 */
socket_t create_addressed_socket(int mode, char *ip_address, short port);

/**
 * @fn socket_t create_listen_socket(char *ip_address, short port)
 * @brief Create a listening socket with the specified address
 * @param ip_address: server IP address to listen to
 * @param port: server TCP port to listen to
 * @return socket created with the specified address and in a listening state
 */
socket_t create_listen_socket(char *ip_address, short port);

/**
 * @fn socket_t accept_client(const socket_t listen_socket)
 * @brief Accept a client connection
 * @param listen_socket: listening socket
 * @return the socket created for the client
 */
socket_t accept_client(const socket_t listen_socket);

/**
 * @fn socket_t connect_to(char *ip_address, short port)
 * @brief Connect to another socket (client or server)
 * @param ip_address: remote IP address
 * @param port: remote port
 * @return the connected socket
 */
socket_t connect_to(char *ip_address, short port);

#endif /* SESSION_H */
