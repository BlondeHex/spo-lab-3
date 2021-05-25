#include "books.h"

int get_books_length(struct book **books) {
    int length = 0;
    while (books[length] != NULL) {
        length += 1;
    }
    return length;
}