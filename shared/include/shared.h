#pragma once
#include <sys/socket.h>

#define MAXDATASIZE 1000

void *get_in_addr(struct sockaddr *sa);

unsigned calculate_polling(unsigned file_size, unsigned buffer_size);

/**
 * Send data via a socket, the first part sends the size of the data, awaits confirmation that the data
 * was received by the receiver and then sends the data.
 * @param socket the socket to connect to.
 * @param data data to be send over the network.
 * @param data_size the size of the given data.
 * @param flags any flags needed for the socket.
 * @return 0 if a success or -1 if failed.
 */
ssize_t send_large(int socket, const void *data, size_t data_size, int flags);

/**
 * Receives a large file, the first part gets the total size, sends an ack that its ready then begins
 * getting all the data.
 * @param socket the socket to listen on.
 * @param buffer the buffer off data to be converted back into what is needed.
 * @param flags to be passed into send()
 * @return the size of the data.
 */
ssize_t receive_large(int socket, char **buffer, int flags);

/**
 * Get the string from a key value in json
 * @param json json object to search.
 * @param key the key to the data.
 * @return data from the json.
 */
const char *get_string_from_key(json_object* json, char *key);

/**
 * Add a string to a json object. Serves as a helper function
 * @param json the main json object to modify
 * @param key the key to insert at.
 * @param data string value to insert.
 */
void add_object_to_json(struct json_object* json, char *key, char* data);