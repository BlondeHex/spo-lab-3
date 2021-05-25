#include "client.h"

int client_socket;

int start_client(char *username, long port) {
    int status = connect_server(CLIENT_IP, port, &client_socket);
    if (status != 0) return status;
    status = get_connect_status(&client_socket);
    if (status != 0) return status;
    struct book **books = malloc(20 * sizeof(struct book));
    get_books_from_server(&client_socket, books);
    int exit_flag = 0;
    int need_to_update = 0;
    pthread_t threadAccept;
    struct pth_listen_args args = {client_socket, &exit_flag, &need_to_update, books};
    pthread_create(&threadAccept, NULL, init_listen_pthread, &args);
    start_ui(books, &client_socket, username, &exit_flag, &need_to_update);
    return status;
}