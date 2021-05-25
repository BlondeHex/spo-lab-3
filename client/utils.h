#ifndef LAB3_UTILS_H
#define LAB3_UTILS_H

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include "../common/network.h"

struct pth_listen_args {
   int socket;
    int *exit_flag;
    int *need_to_update;
    struct book **books;
};


int connect_server(char *username, long port, int *client_socket);

int get_connect_status(const int *client_socket);

void get_books_from_server(const int *client_socket, struct book **books);

void exit_from_server(const int *client_socket);

void rent_book_from_server(const int *client_socket, int book_id, char *username);

void return_book_from_server(const int *client_socket, int book_id, char *username);

void add_book_to_server(const int *client_socket, struct book new_book);

void *init_listen_pthread(void *args);

#endif
