#ifndef LAB3_BOOKS_H
#define LAB3_BOOKS_H

#include <stdio.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>

struct book {
    int id;
    char title[256];
    char authors[256];
    char annotation[256];
    char tags[256];
    int available;
    char get_client[256];
};

int get_books_length(struct book **books);

#endif //LAB3_BOOKS_H
