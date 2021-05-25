#include <string.h>
#include "utils.h"

struct socket sockets[MAX_CLIENT];
int exit_flag = 0;
struct book **books;

void *init_pthread(void *args) {
    struct pth_args *arg = args;
    int connect_socket = arg->connect_socket;
    struct sockaddr *remote_address = (struct sockaddr *) arg->remote_address;
    socklen_t *socket_len = arg->socket_len;
    int *client_number = arg->client_number;
    struct frame frame;

    while (exit_flag != 1) {
        int old_flag = -1;
        int client_socket = accept(connect_socket, remote_address, socket_len);
        for (int i = 0; i < *client_number + 1; i++) {
            if (sockets[i].client_socket == client_socket)
                old_flag = i;
        }
        if (old_flag == -1) {
            sockets[*client_number].client_socket = client_socket;
        }
        if (*client_number < MAX_CLIENT) {
            int client = *client_number;
            if (old_flag != -1) {
                client = old_flag;
            }
            printf("Client %d connected\n", client + 1);
            frame.func = CONNECTED;
            pack(client_socket, &frame);
            if (old_flag == -1) {
                sockets[*client_number].active = 1;
                (*client_number)++;
            } else {
                sockets[old_flag].active = 1;
            }
        } else if (*client_number == MAX_CLIENT) {
            printf("Server is full\n");
            printf("Unknown error\n");
            frame.func = SERVER_FULL;
            pack(sockets[*client_number].client_socket, &frame);
            close(sockets[*client_number].client_socket);
            sockets[*client_number].active = 0;
        } else {
            printf("Unknown error\n");
            frame.func = UNKNOWN_SERVER_ERROR;
            pack(sockets[*client_number].client_socket, &frame);
            close(sockets[*client_number].client_socket);
            sockets[*client_number].active = 0;
        }
    }
    return 0;
}

void client_handler(const int *connect_socket) {
    fd_set read_fds;
    struct timeval tv = {.tv_sec=0, .tv_usec=1000};
    struct frame frame;
    while (exit_flag != 1) {
        FD_ZERO(&read_fds);
        for (int i = 0; i < *connect_socket; i++) {
            FD_SET(sockets[i].client_socket, &read_fds);
            int status = select(sockets[i].client_socket + 1, &read_fds, NULL, NULL, &tv);
            if (status > 0) {
                if (unpack(sockets[i].client_socket, &frame) == 0) {
                    if (frame.func == GET_BOOKS) {
                        send_all_bool(&sockets[i].client_socket);
                    } else if (frame.func == EXIT_CLIENT) {
                        remove_client(i, &sockets[i]);
                    } else if (frame.func == RENT_BOOK) {
                        rent_book(frame.param, frame.username);
                    } else if (frame.func == RETURN_BOOK) {
                        return_book(frame.param, frame.username);
                    } else if (frame.func == ADD_BOOK) {
                        add_book(&sockets[i].client_socket);
                    }
                } else {
                    close(sockets[i].client_socket);
                }
            }
        }
    }
}

int send_all_bool(const int *client_socket) {
    int books_length = get_books_length(books);
    struct frame_book frame_book;
    for (int i = 0; i < books_length; i++) {
        if (i == books_length - 1) {
            frame_book.func = LAST_BOOK;
        } else {
            frame_book.func = GET_BOOKS;
        }
        frame_book.book = *books[i];
        pack_book(*client_socket, &frame_book);
    }
     return 0;
}

int remove_client(const int i, struct socket *client_socket) {
    printf("Client %d exit\n", i + 1);
    close(client_socket->client_socket);
    client_socket->active = 0;
    return 0;
}

void generate_books(size_t size) {
    struct book **arr_books = calloc(size + 1, sizeof(struct book *));
    for (size_t i = 0; i < size; i++) {
        arr_books[i] = malloc(sizeof(struct book));
        arr_books[i]->id = i;
        arr_books[i]->available = 1;
        strcpy(arr_books[i]->get_client, "-");
        strcpy(arr_books[i]->tags, "Horror");
        strcpy(arr_books[i]->title, "The Best Horror");
        strcpy(arr_books[i]->annotation, "Very interesting book");
        strcpy(arr_books[i]->authors, "Stephen King");
    }
    arr_books[size] = NULL;
    books = arr_books;
}

void rent_book(int book_id, char *username) {
    int books_length = get_books_length(books);
    for (int i = 0; i < books_length; i++) {
        if (book_id == books[i]->id) {
            books[i]->available = 0;
            strcpy(books[i]->get_client, username);
        }
    }
    need_to_update();
}

void return_book(int book_id, char *username) {
    int books_length = get_books_length(books);
    for (int i = 0; i < books_length; i++) {
        if (book_id == books[i]->id && strcmp(books[i]->get_client, username) == 0) {
            books[i]->available = 1;
            strcpy(books[i]->get_client, "-");
        }
    }
    need_to_update();
}

void add_book(const int *client_socket) {
    struct frame_book frame_book;
    unpack_book(*client_socket, &frame_book);
    int old_flag = -1;
    int size = get_books_length(books);
    for (int i = 0; i < size; i++) {
        if (frame_book.book.id == books[i]->id)
            old_flag = i;
    }
    if (old_flag != -1) {
        memcpy((books)[(old_flag)], &(frame_book.book), sizeof(struct book));
    } else {
        books = realloc(books, (size + 1) * (sizeof(struct book)));
        books[(size)] = malloc(sizeof(struct book));
        memcpy((books)[(size)], &(frame_book.book), sizeof(struct book));
    }
    need_to_update();
}


void need_to_update() {
    struct frame frame = {.func = NEED_TO_UPDATE};
    send_all(&frame);
}


void stop_server() {
    printf("\nStop server...");
    struct frame frame = {.func = EXIT_SERVER};
    send_all_and_exit(&frame);
    exit_flag = 1;
    exit(SIGINT);
}

void send_all(struct frame *frame) {
    for (size_t i = 0; i < MAX_CLIENT; i++) {
        if (sockets[i].client_socket > -1 && sockets[i].active) {
            pack(sockets[i].client_socket, frame);
        }
    }
}


void send_all_and_exit(struct frame *frame) {
    for (size_t i = 0; i < MAX_CLIENT; i++) {
        if (sockets[i].client_socket > -1 && sockets[i].active) {
            pack(sockets[i].client_socket, frame);
            close(sockets[i].client_socket);
        }
    }
}
