#include "../include/shared.h"
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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

const char *get_string_from_key(json_object* json, char *key) {
    return json_object_get_string(json_object_object_get(json, key));
}

void object_to_json(struct json_object* json, char *key, char* data) {
    json_object *new_object = json_object_new_string(data);
    json_object_object_add(json, key, new_object);
}

void pack_command_to_json(struct json_object* json, char *command) {
    object_to_json(json, "command", command);
}

const char* unpack_command_from_json(struct json_object* json) {
    return get_string_from_key(json, "command");
}

bool check_if_file_exists(const char *file_name) {
    struct stat buffer;
    return (stat (file_name, &buffer) == 0);
}