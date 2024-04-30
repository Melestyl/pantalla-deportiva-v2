#ifndef PANTALLA_DEPORTIVA_V2_SERIALIZATION_H
#define PANTALLA_DEPORTIVA_V2_SERIALIZATION_H

#include <string.h>

/**
 * @def MAX_BUFFER
 * @brief size of a buffer_t for sending/receiving

 */
#define MAX_BUFFER	1024

/**
 *	@typedef	buffer_t
 *	@brief		string to send/receive
 */
typedef char buffer_t[MAX_BUFFER];

/**
 * @struct message_t
 * @brief message structure
 * @var code: message code
 * @var data: optional data corresponding to the code
 */
struct message_t {
	char code;
	buffer_t data;
};

/**
 * @typedef message_t
 * @brief message_t type definition
 */
typedef struct message_t message_t;

/**
 * @fn void deserialize_message(void* serialized_content, void* content)
 * @param content: structure to serialize
 * @param serialized_content: buffer to fill with the serialized message (code + data)
 */
void serialize_message(void* content, void* serialized_content);

/**
 * @fn void deserialize_message(void* content, void* serialized_content)
 * @param content: structure to fill with the deserialized message
 * @param serialized_content: serialized message (code + data)
 */
void deserialize_message(void* content, void* serialized_content);

/**
 * @fn void prepare_message(message_t* message, char code, char* data)
 * @param message: structure to fill with the message
 * @param code: message code
 * @param data: message data
 */
void prepare_message(message_t* message, char code, char* data);
#endif //PANTALLA_DEPORTIVA_V2_SERIALIZATION_H
