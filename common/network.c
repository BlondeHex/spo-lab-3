#include "network.h"

int open_socket(int *connect_socket) {
    *connect_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*connect_socket >= 0) {
        return 0;
    } else {
        printf("Fail open socket\n");
        return -1;
    }
}

int set_socket_name(const int *connect_socket) {
    int reuse = 1;
    if (setsockopt(*connect_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) >= 0) {
        return 0;
    } else {
        printf("Fail set socket name\n");
        return -1;
    }
}

int bind_socket(const int *connect_socket, struct sockaddr_in *server_address) {
    if (bind(*connect_socket, (struct sockaddr *) server_address, sizeof(struct sockaddr_in)) >= 0) {
        return 0;
    } else {
        printf("Fail set socket name\n");
        return -1;
    }
}

int listen_socket(const int *connect_socket, int max_connect) {
    if (listen(*connect_socket, max_connect) >= 0) {
        return 0;
    } else {
        printf("Fail set socket name\n");
        return -1;
    }
}


int unpack(int socket, struct frame *frame) {
    if (read(socket, frame, sizeof(struct frame)) < 1) {
        printf("Can't unpack a frame\n");
        return 1;
    }
    return 0;
}

int pack(int socket, struct frame *frame) {
    if (send(socket, frame, sizeof(struct frame), 0) == -1) {
        printf("Can't pack a frame\n");
        return 1;
    }
    return 0;
}

int pack_book(int socket, struct frame_book *frame_book) {
    if (send(socket, frame_book, sizeof(struct frame_book), 0) == -1) {
        printf("Can't pack a book\n");
        return 1;
    }
    return 0;
}

int unpack_book(int socket, struct frame_book *frame_book) {
    if (read(socket, frame_book, sizeof(struct frame_book)) < 1) {
        printf("Can't unpack a book\n");
        return 1;
    }
    return 0;
}
