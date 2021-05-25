#include "server.h"


int init(int *connect_socket, struct sockaddr_in *server_address, long port) {
    int status = open_socket(connect_socket);
    if (status != 0) return status;

    server_address->sin_family = AF_INET;
    server_address->sin_port = htons(port);
    server_address->sin_addr.s_addr = htonl(INADDR_ANY);

    status = set_socket_name(connect_socket);
    if (status != 0) return status;

    status = bind_socket(connect_socket, server_address);
    if (status != 0) return status;

    status = listen_socket(connect_socket, MAX_CLIENT);
    return status;
}


int start_server() {
    signal(SIGINT, stop_server);
    int connect_socket = 0;
    struct sockaddr_in server_address;
    int status = init(&connect_socket, &server_address, PORT);
    if (status != 0) return status;

    struct sockaddr_in remote_address;
    socklen_t socket_len = sizeof(remote_address);

    int connect_client_number = 0;
    generate_books(6);
    pthread_t threadAccept;
    struct pth_args args = {connect_socket, &remote_address, &socket_len, &connect_client_number};
    pthread_create(&threadAccept, NULL, init_pthread, &args);
    printf("Done.\n");
    client_handler(&connect_client_number);
    return 0;
}
