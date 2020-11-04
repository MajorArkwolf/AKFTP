#include "../include/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <json_tokener.h>
#include <libbase64.h>
#include <time.h>
#include <assert.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../include/directory_handling.h"


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

ssize_t send_large(int socket, const char *data, size_t data_size, int flags) {
    u_int64_t file_size = (u_int64_t) data_size;
    send(socket, &file_size, sizeof(u_int64_t), flags);
    bool rec = false;
    if ((recv(socket, &rec, MAXDATASIZE - 1, flags)) == -1) {
        perror("recv");
        return -1;
    }
    if (!rec) {
        perror("Failed to acknowladge large send.");
        return -1;
    }
    ssize_t sent_size = 0;
    while (file_size != sent_size) {
        ssize_t last_send_size = send(socket, &data[sent_size], file_size - sent_size, flags);
        if (last_send_size == -1) {
            perror("send");
            return -1;
        }
        sent_size += last_send_size;
    }
    return 0;
}

ssize_t receive_large(int socket, char **buffer, int flags) {
    free(*buffer);
    char *buffer2 = NULL;
    u_int64_t neg_file_size = 0;
    bool rec = false;
    if ((recv(socket, &neg_file_size, MAXDATASIZE - 1, flags)) == -1) {
        perror("recv");
        send(socket, &rec, sizeof(bool), flags);
        return -1;
    }
    ssize_t file_size = neg_file_size;
    buffer2 = calloc(file_size + 1, sizeof(char));
    rec = true;
    if (send(socket, &rec, sizeof(bool), flags) == -1) {
        perror("send");
        return -1;
    }

    ssize_t current_size = 0;
    while (file_size != current_size) {
        ssize_t rec_size = recv(socket, &buffer2[current_size], file_size - current_size, 0);
        if (rec_size == -1) {
            perror("recv");
            return -1;
        }
        current_size += rec_size;
    }
    *buffer = buffer2;
    return file_size;
}

const char *get_string_from_key(json_object *json, char *key) {
    return json_object_get_string(json_object_object_get(json, key));
}

void object_to_json(struct json_object *json, char *key, char *data) {
    json_object *new_object = json_object_new_string(data);
    json_object_object_add(json, key, new_object);
}

void pack_command_to_json(struct json_object *json, char *command) {
    object_to_json(json, "command", command);
}

const char *unpack_command_from_json(struct json_object *json) {
    return get_string_from_key(json, "command");
}

bool check_if_file_exists(const char *file_name) {
    struct stat buffer;
    return (stat(file_name, &buffer) == 0);
}

int serialize_file(json_object *json) {
    FILE *fp = NULL;
    uint64_t lSize = 0;
    char *buffer = NULL;
    const char *filename = json_object_get_string(json_object_object_get(json, "filename"));
    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error: File did not open.");
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);
    /* allocate memory for entire content */
    buffer = calloc(lSize + 1, sizeof(char));
    if (!buffer) {
        fclose(fp), fputs("memory alloc fails", stderr), exit(1);
        return -1;
    }
    /* copy the file into the buffer */
    if (1 != fread(buffer, lSize, 1, fp)) {
        fclose(fp), free(buffer), fputs("entire read fails", stderr), exit(1);
        return -1;
    }
    fclose(fp);
    size_t new_size = 0;
    char *encoded_string = encode_string(buffer, lSize, &new_size);
    json_object *file_data = json_object_new_string(encoded_string);
    json_object_object_add(json, "filedata", file_data);
    json_object *de_size = json_object_new_uint64(lSize);
    json_object_object_add(json, "desize", de_size);
    json_object *en_size = json_object_new_uint64(new_size);
    json_object_object_add(json, "ensize", en_size);
    free(buffer);
    free(encoded_string);
    return 0;
}

int deserialize_file(json_object *json, json_object *response) {
    int errorNumber = 0;
    const char *filename = json_object_get_string(json_object_object_get(json, "filename"));
    const char *encoded_data = json_object_get_string(json_object_object_get(json, "filedata"));
    uint64_t en_size = json_object_get_uint64(json_object_object_get(json, "ensize"));
    uint64_t de_size = json_object_get_uint64(json_object_object_get(json, "desize"));
    size_t new_size = (size_t) de_size;
    if (encoded_data == NULL) {
        perror("File data not found.");
        return -1;
    }
    char *filedata = decode_string(encoded_data, en_size, &new_size);
    FILE *fp = NULL;
    fp = fopen(filename, "w");
    if (fp == NULL) {
        errorNumber = -1;
        json_object *error = json_object_new_int(errorNumber);
        fclose(fp);
        json_object_object_add(response, "error", error);
        return -1;
    }
    fwrite(filedata, de_size, 1, fp);
    fclose(fp);
    json_object *error = json_object_new_int(errorNumber);
    json_object_object_add(response, "error", error);
    free(filedata);
    return 0;
}

void request_file(const int socket, json_object *json, const char *file_location) {
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
    json_object_put(response);
}

char *encode_string(const char *input, size_t input_size, size_t *output_size) {
    char *output = NULL;
    struct base64_state state;
    *output_size = input_size * 1.40;
    output = calloc(*output_size, sizeof(char));
    base64_stream_encode_init(&state, 0);
    base64_encode(input, input_size, output, output_size, 0);
    return output;
}

char *decode_string(const char *input, size_t input_size, size_t *output_size) {
    char *output = NULL;
    struct base64_state state;
    base64_stream_decode_init(&state, 0);
    output = calloc(*output_size, sizeof(char));
    base64_decode(input, input_size, output, output_size, 0);
    return output;
}

void log_to_file(const char *wd, json_object *from_client, json_object *to_client, int socket) {
    char file_dir[1000] = {""};
    strcpy(file_dir, wd);
    strcat(file_dir, "/log.txt");
    FILE *fptr = fopen(file_dir, "a");
    if (fptr == NULL) {
        perror("File failed to open.");
        return;
    }
    char output[10000] = {""};
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    assert(strftime(output, sizeof(output), "%c", tm) > 0);
    strcat(output, ": ");
    struct sockaddr_in peer;
    socklen_t addressLength = sizeof(peer);
    if (getpeername(socket, (struct sockaddr *) &peer, &addressLength) == -1) {
        perror("getpeername() failed");
        return;
    }
    strcat(output, inet_ntoa(peer.sin_addr));
    strcat(output, ": ");
    if (from_client == NULL) {
        //ERROR GOES HERE
        strcat(output, "ERROR, Clients message did not parse.");
    } else {
        const char *command = unpack_command_from_json(from_client);
        strcat(output, command);
        strcat(output, " ");
        if (strcmp(command, "pwd") == 0) {
            //PWD current working directory
            if (to_client != NULL) {
                const char *cwd = json_object_get_string(json_object_object_get(to_client, "cwd"));
                if (cwd != NULL) {
                    strcat(output, cwd);
                }
            }
        } else if (strcmp(command, "cd") == 0) {
            //Change directory
            const char *dir_change = json_object_get_string(json_object_object_get(from_client, "dir"));
            if (dir_change != NULL) {
                strcat(output, dir_change);
            }
        } else if (strcmp(command, "dir") == 0) {
            //Dir gives file names
            if (to_client != NULL) {
                const char *current_directory = json_object_get_string(
                        json_object_object_get(to_client, "currentDirectory"));
                if (current_directory != NULL) {
                    strcpy(output, current_directory);
                }
            }
        } else if (strcmp(command, "put") == 0) {
            //Client to server file.
            const char *file_name = json_object_get_string(json_object_object_get(from_client, "filename"));
            if (file_name != NULL) {
                int error = 0;
                char *work_dir = GetCurrentWorkingDirectory(&error);
                strcat(output, work_dir);
                strcat(output, "/");
                strcat(output, file_name);
                free(work_dir);
            }
        } else if (strcmp(command, "get") == 0) {
            //Server to client.
            const char *file_name = json_object_get_string(json_object_object_get(from_client, "filename"));
            if (file_name != NULL) {
                int error = 0;
                char *work_dir = GetCurrentWorkingDirectory(&error);
                strcat(output, work_dir);
                strcat(output, "/");
                strcat(output, file_name);
                free(work_dir);
            }
        }
        if (to_client != NULL) {
            if (json_object_object_get(from_client, "error") == NULL) {
                char s_num[5];
                int error = json_object_get_int(json_object_object_get(from_client, "error"));
                sprintf(s_num, "%d", error);
                strcat(output, " error code: ");
                strcat(output, s_num);
            }
        }
    }
    fprintf(fptr, "%s\n", output);
    fclose(fptr);
}