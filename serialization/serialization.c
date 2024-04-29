#include "serialization.h"

/**
 * @fn void deserialize_message(void* serialized_content, void* content)
 * @param content: structure to serialize
 * @param serialized_content: buffer to fill with the serialized message (code + data)
 */
void serialize_message(void* content, void* serialized_content) {
	// Casting
	message_t *message = (message_t *) content;
	char *serialized_message = (char *) serialized_content;

	// Serializing
	serialized_message[0] = message->code;
	strcpy(serialized_message + 1, message->data);

	// Adding the null character
	serialized_message[strlen(serialized_message)] = '\0';
}

/**
 * @fn void deserialize_message(void* content, void* serialized_content)
 * @param content: structure to fill with the deserialized message
 * @param serialized_content: serialized message (code + data)
 */
void deserialize_message(void* content, void* serialized_content) {
	// Casting
	message_t *message = (message_t *) content;
	char *serialized_message = (char *) serialized_content;

	// Deserializing
	message->code = serialized_message[0];
	strcpy(message->data, serialized_message + 1);

	// Adding the null character
	message->data[strlen(message->data)] = '\0';
}