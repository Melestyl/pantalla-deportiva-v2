/**
 * @file data.h
 * @brief Data Representation Layer Specification
 * @date 2024-04-24
 * @version 1.0
 * @authors
 * 	- TELLIER--CALOONE Tom
 * 	- DELANNOY Anaël
 */

#ifndef DATA_H
#define DATA_H
/*
*****************************************************************************************
 *			S P E C I F I C   I N C L U D E S
 */
#include "session.h"
/*
*****************************************************************************************
 *			C O N S T A N T S   D E F I N I T I O N
 */
/**
 *	@def		MAX_BUFFER
 *	@brief		size of a buffer_t for sending/receiving
 */
#define MAX_BUFFER	1024
/*
*****************************************************************************************
 * 		D A T A   S T R U C T U R E S
 */
/**
 *	@typedef	buffer_t
 *	@brief		string to send/receive
 */
typedef char buffer_t[MAX_BUFFER];
/**
 *	@typedef	generic
 *	@brief		generic data type: requests/responses
 */
typedef void * generic;
/**
 *	@typedef	function_pointer
 *	@brief		pointer to a generic function with 2 generic parameters
 */
typedef void (*function_pointer) (generic, generic);
/*
*****************************************************************************************
 *			F U N C T I O N   P R O T O T Y P E S
 */

/**
 * @fn void send_message(socket_t *exchange_socket, generic content, function_pointer serializer_fct, ...)
 * @brief send a request/response on a socket (stream or datagram)
 * @param exchange_socket: exchange socket to use for sending
 * @param content: request/response to serialize before sending
 * @param serializer_fct: pointer to the request/response serialization function
 * @param ...: if the mode is DGRAM, the call requires the IP address and the port
 * @note if the serializer_fct parameter is NULL then content is a string
 * @note if the mode is DGRAM, the call requires the IP address and the port
 * @result exchange_socket parameter modified for the DGRAM mode
 */
void send_message(socket_t *exchange_socket, generic content, function_pointer serializer_fct, ...);

/**
 * @fn void receive_message(socket_t *exchange_socket, generic content, function_pointer deserializer_fct)
 * @brief receive a request/response on a socket (stream or datagram)
 * @param exchange_socket: exchange socket to use for receiving
 * @param content:	request/response received after deserializing the reception buffer
 * @param deserializer_fct:	pointer to the request/response deserialization function
 * @note if the deserializer_fct parameter is NULL then content is a string
 * @result content parameter modified with the received request/response
 */
void receive_message(socket_t *exchange_socket, generic content, function_pointer deserializer_fct);

#endif /* DATA_H */
