#include "../include/shared.h"
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

unsigned calculate_polling(unsigned file_size, unsigned buffer_size) {
    int polling = file_size / buffer_size;
    if (file_size % buffer_size != 0) {
        ++polling;
    }
    return polling;
}
/**
 * Send data via a socket, the first part sends the size of the data, awaits confirmation that the data
 * was received by the receiver and then sends the data.
 * @param socket the socket to connect to.
 * @param data data to be send over the network.
 * @param data_size the size of the given data.
 * @param flags any flags needed for the socket.
 * @return 0 if a success or -1 if failed.
 */
ssize_t send_large(int socket, const void *data, size_t data_size, int flags) {
    unsigned file_size = data_size;
    send(socket, &file_size, sizeof(int), flags);
    bool rec = false;
    if ((recv(socket, &rec, MAXDATASIZE-1, flags)) == -1) {
        perror("recv");
        return -1;
    }
    if (!rec) {
        perror("Failed to acknowladge large send.");
        return -1;
    }
    send(socket, data, file_size, flags);
    return 0;
}
/**
 * Receives a large file, the first part gets the total size, sends an ack that its ready then begins
 * getting all the data.
 * @param socket the socket to listen on.
 * @param buffer the buffer off data to be converted back into what is needed.
 * @param flags to be passed into send()
 * @return the size of the data.
 */
ssize_t receive_large(int socket, char **buffer, int flags) {
    free(*buffer);
    unsigned file_size = 0;
    bool rec = false;
    if ((recv(socket, &file_size, MAXDATASIZE-1, flags)) == -1) {
        perror("recv");
        send(socket, &rec, sizeof(bool), flags);
        return -1;
    }
    *buffer = calloc(file_size, sizeof(char));
    rec = true;
    send(socket, &rec, sizeof(bool), flags);
    if ((recv(socket, *buffer, file_size, 0)) == -1) {
        perror("recv");
        return -1;
    }
    return file_size;
}