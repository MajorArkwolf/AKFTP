#pragma once
#include <sys/socket.h>

#define MAXDATASIZE 1000

void *get_in_addr(struct sockaddr *sa);

unsigned calculate_polling(unsigned file_size, unsigned buffer_size);

ssize_t send_large(int socket, const void *data, size_t data_size, int flags);

ssize_t receive_large(int socket, char **buffer, int flags);