#ifndef LAB3_UTILS_H
#define LAB3_UTILS_H

#include <stdio.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include "../common/network.h"

#define MAX_CLIENT 8

struct pth_args {
    int connect_socket;
    struct sockaddr_in *remote_address;
    socklen_t *socket_len;
    int *client_number;
};

struct socket {
    int client_socket;
    int active;
};

void *init_pthread(void *args);

void generate_books(size_t size);

int send_all_bool(const int *client_socket);

void client_handler(const int *connect_socket);

int remove_client(const int i, struct socket *client_socket);

void stop_server();

void send_all(struct frame *frame);

void need_to_update();

void rent_book(int book_id, char *username);

void return_book(int book_id, char *username);

void add_book(const int *client_socket);

void send_all_and_exit(struct frame *frame);

#endif //LAB3_UTILS_H
