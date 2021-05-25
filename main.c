#include "main.h"

int main(int argc, char **argv) {
    int status;
    if (argc == 2 && strcmp(argv[1], SERVER) == 0) {
        printf("Start server....\n");
        status = start_server();
    } else if (argc == 4 && strcmp(argv[1], CLIENT) == 0) {
        long port = strtol(argv[3], NULL, 10);
        if (port <= MAX_VALUE_PORT && port >= MIN_VALUE_PORT) {
            printf("Start client...\n");
            status = start_client(argv[2], port);
        } else {
            printf("Incorrect port\n");
            status = -1;
        }
    } else {
        printf("Invalid argument\n");
        status = -1;
    }
    return status;
}
