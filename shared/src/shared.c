#include "../include/shared.h"
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <json_tokener.h>


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

ssize_t send_large(int socket, const void *data, size_t data_size, int flags) {
    u_int32_t file_size = (u_int32_t)data_size;
    send(socket, &file_size, sizeof(u_int32_t), flags);
    bool rec = false;
    if ((recv(socket, &rec, MAXDATASIZE-1, flags)) == -1) {
        perror("recv");
        return -1;
    }
    if (!rec) {
        perror("Failed to acknowladge large send.");
        return -1;
    }
    u_int32_t sent_size = 0;
    while (file_size != sent_size) {
        u_int32_t last_send_size = (u_int32_t)send(socket, data, file_size, flags);
        if (last_send_size == -1) {
            perror("send");
            return -1;
        }
        sent_size += last_send_size;
    }
    return 0;
}

ssize_t receive_large(int socket, unsigned char **buffer, int flags) {
    free(*buffer);
    u_int32_t file_size = 0;
    bool rec = false;
    if ((recv(socket, &file_size, MAXDATASIZE-1, flags)) == -1) {
        perror("recv");
        send(socket, &rec, sizeof(bool), flags);
        return -1;
    }
    *buffer = calloc(file_size, sizeof(unsigned char));
    rec = true;
    if (send(socket, &rec, sizeof(bool), flags) == -1) {
        perror("send");
        return -1;
    }
    u_int32_t current_size = 0;

    while (file_size != current_size) {
        u_int32_t rec_size = (u_int32_t)recv(socket, *buffer, file_size, 0);
        if (rec_size == -1) {
            perror("recv");
            return -1;
        }
        current_size += rec_size;
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
void serialize_file(json_object *json) {
    FILE *fp = NULL;
    size_t size = 0;
    long lSize;
    char *buffer;
    const char *filename = json_object_get_string(json_object_object_get(json, "filename"));
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Error: File did not open.");
        return;
    }
    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);
/* allocate memory for entire content */
    buffer = calloc(1, lSize + 1);
    if (!buffer) {
        fclose(fp), fputs("memory alloc fails", stderr), exit(1);
    }
/* copy the file into the buffer */
    if (1 != fread(buffer, lSize, 1, fp)) {
        fclose(fp), free(buffer), fputs("entire read fails", stderr), exit(1);
    }
    fclose(fp);

    //char *dec = b64_encode(buffer, strlen(buffer));
    json_object *filedata = json_object_new_string(buffer);
    json_object_object_add(json, "filedata", filedata);
    free(buffer);
}

void deserialize_file(json_object *json, json_object *response) {
    int errorNumber = 0;
    const char *filename = json_object_get_string(json_object_object_get(json, "filename"));
    const char *filedata = json_object_get_string(json_object_object_get(json, "filedata"));
    //unsigned char* decoded = b64_decode(filedata, sizeof(filedata));
    //char * char_decoded = b64_buf_realloc(decoded, sizeof(decoded));
    FILE *fp = NULL;
    fp = fopen(filename, "w");
    if (fp == NULL) {
        errorNumber = -1;
        json_object *error = json_object_new_int(errorNumber);
        fclose(fp);
        json_object_object_add(response, "error", error);
        return;
    }
    fputs(filedata, fp);
    fclose(fp);
    json_object *error = json_object_new_int(errorNumber);
    json_object_object_add(response, "error", error);
}

void request_file(const int socket, json_object *json, const char* file_location) {
    json_object *json_file = json_object_new_string(file_location);
    json_object_object_add(json, "filename", json_file);
    size_t size = 0;
    const char *data = json_object_to_json_string_length(json, 0, &size);
    send_large(socket, data, size, 0);
    char *response_data = NULL;
    receive_large(socket, &response_data, 0);
    if (response_data == NULL) {
        perror("Server responded with no data");
        return;
    }
    json_object *response = json_tokener_parse(response_data);
    free(response_data);
    if (response == NULL) {
        perror("Failed to construct JSON from server data.");
        return;
    }
    deserialize_file(response, response);
}