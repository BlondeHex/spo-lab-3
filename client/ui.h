#ifndef LAB3_UI_H
#define LAB3_UI_H

#include <ncurses.h>
#include "../common/books.h"
#include "utils.h"
//#include <form.h>

#define CHOOSE_MODE 0
#define ACTIVE_MODE 1
#define EDIT_MODE 2
#define WRITE_MODE 3
#define SEARCH_MODE 4


void exit_client(WINDOW *win_info, WINDOW *win_main, WINDOW *win_list, struct book **books);

void start_ui(struct book **books, const int *client_socket, char *username, int *exit_flag, int *need_to_update);

void init_ui(struct book **books, const int *client_socket, char *username, int *exit_flag, int *need_to_update);

int init_curses();

void free_books(struct book **books);

void show_book_info(WINDOW *win_info, struct book **books);

void print_book(WINDOW *win_list, struct book **books);

void print_menu(WINDOW *win_main, struct book **books, char *username);

void print_row(char *call_row, char *row, int *x, int *y, WINDOW *win_info);

void update_view(WINDOW *win_info, WINDOW *win_main, WINDOW *win_list, struct book **books, char *username);

void clear_window(WINDOW *win_info, WINDOW *win_main, WINDOW *win_list);

void print_border(WINDOW *win_info, WINDOW *win_main, WINDOW *win_list);

void clear_full_window(WINDOW *win_info, WINDOW *win_main, WINDOW *win_list);

void edit_book_info(WINDOW *win_info, struct book book);

void input(WINDOW *win_info, WINDOW *win_main, WINDOW *win_list, struct book **books, char *username);

struct book new_book(struct book **books);

void search(WINDOW *win_info);

void input_search(WINDOW *win_info, WINDOW *win_main, WINDOW *win_list, struct book **books, char *username);


int check_contain(struct book *book);

#endif //LAB3_UI_H
