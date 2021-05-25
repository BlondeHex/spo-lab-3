#include <string.h>
#include "utils.h"

int connect_server(char *ip, long port, int *client_socket) {
    int status = open_socket(client_socket);
    if (status != 0) return status;

    struct sockaddr_in server_address = {.sin_family = AF_INET, .sin_port = htons(port)};
    struct in_addr sin_addr;
    inet_aton(ip, &sin_addr);
    server_address.sin_addr = sin_addr;
    printf("Connect to server...\n");
    status = connect(*client_socket, (struct sockaddr *) &server_address, sizeof(struct sockaddr_in));
    if (status != 0) return status;
    return 0;
}

int get_connect_status(const int *client_socket) {
    struct frame frame;
    unpack(*client_socket, &frame);
    if (frame.func == SERVER_FULL || frame.func == UNKNOWN_SERVER_ERROR) {
        printf("Can't connect to server.\n");
        close(*client_socket);
        return frame.func;
    }
    if (frame.func == CONNECTED)
        printf("Connect.\n");
    return 0;
}

void get_books_from_server(const int *client_socket, struct book **books) {
    struct frame frame = {.func= GET_BOOKS};
    pack(*client_socket, &frame);
    int arr_length = 0;
    int stop_flag = 0;
    struct frame_book frame_book;
    while (!stop_flag) {
        unpack_book(*client_socket, &frame_book);
        books = realloc(books, (arr_length + 1) * (sizeof(struct book)));
        books[(arr_length)] = malloc(sizeof(struct book));
        memcpy((books)[(arr_length)], &(frame_book.book), sizeof(struct book));
        arr_length += 1;
        if (frame_book.func == LAST_BOOK) {
            stop_flag = 1;
            books[(arr_length)] = NULL;
        }
    }

}

void rent_book_from_server(const int *client_socket, int book_id, char *username) {
    struct frame frame;
    frame.func = RENT_BOOK;
    frame.param = book_id;
    strcpy(frame.username, username);
    pack(*client_socket, &frame);
}

void return_book_from_server(const int *client_socket, int book_id, char *username) {
    struct frame frame;
    frame.func = RETURN_BOOK;
    frame.param = book_id;
    strcpy(frame.username, username);
    pack(*client_socket, &frame);
}


void add_book_to_server(const int *client_socket, struct book new_book) {
    struct frame frame;
    frame.func = ADD_BOOK;
    pack(*client_socket, &frame);
    struct frame_book frame_with_book;
    frame_with_book.book = new_book;
    frame_with_book.func = ADD_BOOK;
    pack_book(*client_socket, &frame_with_book);
}

void exit_from_server(const int *client_socket) {
    struct frame frame = {.func= EXIT_CLIENT};
    pack(*client_socket, &frame);
}

void *init_listen_pthread(void *args) {
    struct pth_listen_args *arg = args;
    struct frame frame;
    while (!*arg->exit_flag) {
        unpack(arg->socket, &frame);
        if (frame.func == NEED_TO_UPDATE) {
            get_books_from_server(&arg->socket, arg->books);
            *(arg->need_to_update) = 1;
        } else if (frame.func == EXIT_SERVER) {
            *(arg->exit_flag) = 1;
        }
    }
}