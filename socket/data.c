/**
 * @file data.c
 * @brief Data Representation Layer Implementation
 * @date 2023-04-24
 * @version 1.0
 * @authors
 * 	- TELLIER--CALOONE Tom
 * 	- DELANNOY Anaël
 */

#include "data.h"

/**
 * @fn void send_stream_message(socket_t *exchange_socket, buffer_t content)
 * @brief send a message on a stream socket
 * @param exchange_socket: exchange socket to use for sending
 * @param content: content to send
 */
void send_stream_message(socket_t *exchange_socket, buffer_t content) {
	CHECK(write(exchange_socket->file_descriptor, content, strlen(content)), "Can't send STREAM message")
}

/**
 * @fn void send_dgram_message(socket_t *exchange_socket, buffer_t content, char* ip, int port)
 * @brief send a message on a datagram socket
 * @param exchange_socket: exchange socket to use for sending
 * @param content: content to send
 * @param ip: sender's IP address
 * @param port: sender's port
 */
void send_dgram_message(socket_t *exchange_socket, buffer_t content, char* ip, int port) {
	struct sockaddr_in dest_addr;
	int content_length = strlen(content);

	// Setting up the destination address
	adr2struct(&dest_addr, ip, port);

	// Using sendto
	CHECK(sendto(
			exchange_socket->file_descriptor,
			content,
			content_length,
			0,
			(struct sockaddr *)&dest_addr,
					sizeof(dest_addr)),
		  "Can't send DGRAM message"
		  );
}

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
void send_message(socket_t *exchange_socket, generic content, function_pointer serializer_fct, ...) {
	buffer_t serialized_content;

	// Serializing
	if (serializer_fct != NULL)
		serializer_fct(content, serialized_content);
	else
		strcpy(serialized_content, (char *) content);

	// Sending
	if (exchange_socket->mode == SOCK_STREAM)
		send_stream_message(exchange_socket, serialized_content);
	else {
		va_list pArg;
		va_start(pArg, serializer_fct);
		char *ip = va_arg(pArg, char *);
		int port = va_arg(pArg, int);
		send_dgram_message(exchange_socket, serialized_content, ip, port);
		va_end(pArg);
	}
}

/**
 * @fn void receive_stream_message(socket_t *exchange_socket, buffer_t content)
 * @brief receive a message on a stream socket
 * @param exchange_socket: exchange socket to use for receiving
 * @param content: received content
 */
void receive_stream_message(socket_t *exchange_socket, buffer_t content) {
	// Clearing the buffer
	memset(content, 0, sizeof(buffer_t));

	// Using read to receive data
	CHECK(read(exchange_socket->file_descriptor, content, sizeof(buffer_t) - 1), "Can't read STREAM message");

	// Ending received data with \0
	content[strlen(content)] = '\0';
}

/**
 * @fn void receive_dgram_message(socket_t *exchange_socket, buffer_t content)
 * @brief receive a message on a datagram socket
 * @param exchange_socket: exchange socket to use for receiving
 * @param content: received content
 */
void receive_dgram_message(socket_t *exchange_socket, buffer_t content) {
	struct sockaddr_in exp_addr;
	socklen_t addr_len = sizeof(exp_addr);

	// Clearing the buffer
	memset(content, 0, sizeof(buffer_t));

	// Using recvfrom to receive data
	CHECK(recvfrom(exchange_socket->file_descriptor, content, sizeof(buffer_t) - 1, 0, (struct sockaddr *)&exp_addr, &addr_len), "Can't receive DGRAM message");

	// Ending received data with \0
	content[strlen(content)] = '\0';
}

/**
 * @fn void receive_message(socket_t *exchange_socket, generic content, function_pointer deserializer_fct)
 * @brief receive a request/response on a socket (stream or datagram)
 * @param exchange_socket: exchange socket to use for receiving
 * @param content:	request/response received after deserializing the reception buffer
 * @param deserializer_fct:	pointer to the request/response deserialization function
 * @note if the deserializer_fct parameter is NULL then content is a string
 * @result content parameter modified with the received request/response
 */
void receive_message(socket_t *exchange_socket, generic content, function_pointer deserializer_fct) {
	buffer_t deserialized_content;

	// Receiving
	if (exchange_socket->mode == SOCK_STREAM)
		receive_stream_message(exchange_socket, deserialized_content);
	else
		receive_dgram_message(exchange_socket, deserialized_content);

	// Deserializing
	if (deserializer_fct != NULL)
		deserializer_fct(content, deserialized_content);
	else
		strcpy((char*) content, deserialized_content);
}

