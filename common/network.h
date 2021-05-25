#ifndef LAB3_NETWORK_H
#define LAB3_NETWORK_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "books.h"


#define CONNECTED 0
#define SERVER_FULL 1
#define UNKNOWN_SERVER_ERROR 2
#define GET_BOOKS 3
#define LAST_BOOK 4
#define EXIT_CLIENT 5
#define EXIT_SERVER 6
#define NEED_TO_UPDATE 7
#define RENT_BOOK 8
#define RETURN_BOOK 9
#define ADD_BOOK 10

struct frame {
    int func;
    int param;
    char username[256]
};

struct frame_book {
    unsigned char func;
    struct book book;
};

int open_socket(int *connect_socket);

int set_socket_name(const int *connect_socket);

int bind_socket(const int *connect_socket, struct sockaddr_in *server_address);

int listen_socket(const int *connect_socket, int max_connect);

int pack(int socket, struct frame *frame);

int unpack(int socket, struct frame *frame);

int pack_book(int socket, struct frame_book *frame_book);

int unpack_book(int socket, struct frame_book *frame_book);

#endif
