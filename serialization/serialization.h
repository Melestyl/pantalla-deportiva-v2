#ifndef PANTALLA_DEPORTIVA_V2_SERIALIZATION_H
#define PANTALLA_DEPORTIVA_V2_SERIALIZATION_H

#include <string.h>

/**
 * @struct message_t
 * @brief message structure
 * @var code: message code
 * @var data: optional data corresponding to the code
 */
struct message_t {
	char code;
	char *data;
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

#endif //PANTALLA_DEPORTIVA_V2_SERIALIZATION_H
