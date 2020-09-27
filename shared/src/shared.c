#include "../include/shared.h"
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

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

ssize_t send_large(int socket, const void *data, size_t data_size, int flags) {
    unsigned size = sizeof(char) * data_size;
    send(socket, &size, sizeof(int), flags);
    bool rec = false;
    if ((recv(socket, &rec, MAXDATASIZE-1, flags)) == -1) {
        perror("recv");
        return -1;
    }
    if (!rec) {
        perror("Failed to acknowladge large send.");
        return -1;
    }
    send(socket, &data, size - 1, flags);
    return 0;
}

ssize_t receive_large(int socket, void *buffer, int flags) {
    unsigned file_size = 0;
    bool rec = false;
    if ((recv(socket, &file_size, MAXDATASIZE-1, flags)) == -1) {
        perror("recv");
        send(socket, &rec, MAXDATASIZE, flags);
        return -1;
    }
    rec = true;
    send(socket, &rec, sizeof(bool), flags);
    if ((recv(socket, &buffer, file_size - 1, 0)) == -1) {
        perror("recv");
        return -1;
    }
    return file_size / sizeof(char);
}