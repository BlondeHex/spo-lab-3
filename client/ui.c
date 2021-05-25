#include <string.h>
#include "ui.h"

int selected_book = 0;
int selected_edit_row = 0;
int active_tab = CHOOSE_MODE;
int search_tab = 0;
char search_input[256] = "";
struct book edit_book;

void start_ui(struct book **books, const int *client_socket, char *username, int *exit_flag, int *need_to_update) {
    if (init_curses() == 0) {
        init_ui(books, client_socket, username, exit_flag, need_to_update);
    }
}

int init_curses() {
    if (!initscr()) {
        return 1;
    }
    noecho();
    cbreak();
    return 0;
}

void init_ui(struct book **books, const int *client_socket, char *username, int *exit_flag, int *need_to_update) {
    WINDOW *win_list = newwin(30, 40, 1, 1);
    WINDOW *win_info = newwin(30, 60, 1, 41);
    WINDOW *win_main = newwin(32, 102, 0, 0);
    refresh();
    update_view(win_info, win_main, win_list, books, username);

    char ch;
    while (ch = wgetch(win_main)) {
        int length = get_books_length(books) - 1;
        if (*exit_flag == 1) exit_client(win_info, win_main, win_list, books);
        if (*need_to_update == 1) {
            update_view(win_info, win_main, win_list, books, username);
            *need_to_update = 0;
        }
        switch (ch) {
            case 'w':
                if (active_tab == CHOOSE_MODE) {
                    if (selected_book != 0) {
                        selected_book -= 1;
                        update_view(win_info, win_main, win_list, books, username);
                    }
                }
                if (active_tab == EDIT_MODE) {
                    if (selected_edit_row != 0) {
                        selected_edit_row -= 1;
                        update_view(win_info, win_main, win_list, books, username);
                    }
                }
                if (active_tab == SEARCH_MODE) {
                    if (search_tab != 0) {
                        search_tab -= 1;
                        update_view(win_info, win_main, win_list, books, username);
                    }
                }
                break;
            case 'e':
                if (active_tab == CHOOSE_MODE) {
                    if (selected_book != length) {
                        selected_book += 1;
                        update_view(win_info, win_main, win_list, books, username);
                    }
                }
                if (active_tab == ACTIVE_MODE) {
                    active_tab = EDIT_MODE;
                    edit_book = *books[selected_book];
                    update_view(win_info, win_main, win_list, books, username);
                }
                if (active_tab == EDIT_MODE) {
                    if (selected_edit_row != 3) {
                        selected_edit_row += 1;
                        update_view(win_info, win_main, win_list, books, username);
                    }
                }
                if (active_tab == SEARCH_MODE) {
                    if (search_tab != 3) {
                        search_tab += 1;
                        update_view(win_info, win_main, win_list, books, username);
                    }
                }
                break;
            case 's':
                if (active_tab == CHOOSE_MODE) {
                    active_tab = ACTIVE_MODE;
                    update_view(win_info, win_main, win_list, books, username);
                }
                if (active_tab == EDIT_MODE) {
                    active_tab = CHOOSE_MODE;
                    add_book_to_server(client_socket, edit_book);
                    update_view(win_info, win_main, win_list, books, username);
                }
                if (active_tab == SEARCH_MODE) {
                    input_search(win_info, win_main, win_list, books, username);
                }
                break;
            case 'b':
                if (active_tab == ACTIVE_MODE) {
                    active_tab = CHOOSE_MODE;
                    update_view(win_info, win_main, win_list, books, username);
                }
                if (active_tab == EDIT_MODE) {
                    active_tab = ACTIVE_MODE;
                    update_view(win_info, win_main, win_list, books, username);
                }
                if (active_tab == SEARCH_MODE) {
                    active_tab = CHOOSE_MODE;
                    update_view(win_info, win_main, win_list, books, username);
                }
                break;
            case 'a':
                if (active_tab == CHOOSE_MODE) {
                    active_tab = EDIT_MODE;
                    edit_book = new_book(books);
                    update_view(win_info, win_main, win_list, books, username);
                }
                break;
            case 'g':
                if (active_tab == ACTIVE_MODE) {
                    if (books[selected_book]->available == 1) {
                        rent_book_from_server(client_socket, selected_book, username);
                    }
                    if (books[selected_book]->available == 0 &&
                        strcmp(books[selected_book]->get_client, username) == 0) {
                        return_book_from_server(client_socket, selected_book, username);
                    }
                }
                break;
            case 'c':
                if (active_tab == EDIT_MODE) {
                    active_tab = WRITE_MODE;
                    input(win_info, win_main, win_list, books, username);
                }
                break;
            case 't':
                if (active_tab == CHOOSE_MODE) {
                    active_tab = SEARCH_MODE;
                    update_view(win_info, win_main, win_list, books, username);
                }
                break;
            case 'q':
                *exit_flag = 1;
                exit_from_server(client_socket);
                exit_client(win_info, win_main, win_list, books);
                break;
        }
    }
    getch();
    endwin();

}

void exit_client(WINDOW *win_info, WINDOW *win_main, WINDOW *win_list, struct book **books) {
    clear_full_window(win_info, win_main, win_list);
    delwin(win_list);
    delwin(win_info);
    delwin(win_main);
    free_books(books);
    exit(0);
}

void free_books(struct book **books) {
    int count_book = get_books_length(books);
    for (int i = 0; i < count_book; i++) {
        if (books[i]) free(books[i]);
    }
    free(books);
}

struct book new_book(struct book **books) {
    int length = get_books_length(books);
    struct book new_book;
    new_book.id = length;
    new_book.available = 1;
    char buffer[256] = "";
    memcpy(new_book.title, buffer, 256);
    memcpy(new_book.annotation, buffer, 256);
    memcpy(new_book.authors, buffer, 256);
    memcpy(new_book.tags, buffer, 256);
    memcpy(new_book.get_client, buffer, 256);
    return new_book;
}

void update_view(WINDOW *win_info, WINDOW *win_main, WINDOW *win_list, struct book **books, char *username) {
    clear_window(win_info, win_main, win_list);
    if (active_tab == EDIT_MODE) { edit_book_info(win_info, edit_book); }
    else if (active_tab == SEARCH_MODE) { search(win_info); }
    else {
        show_book_info(win_info, books);
    }
    print_book(win_list, books);
    print_menu(win_main, books, username);
    wrefresh(win_main);
    wrefresh(win_info);
    wrefresh(win_list);
    curs_set(0);
}

void clear_window(WINDOW *win_info, WINDOW *win_main, WINDOW *win_list) {
    wclear(win_list);
    wclear(win_main);
    wclear(win_info);
    print_border(win_info, win_main, win_list);
    wrefresh(win_main);
    wrefresh(win_info);
    wrefresh(win_list);
}

void clear_full_window(WINDOW *win_info, WINDOW *win_main, WINDOW *win_list) {
    wclear(win_list);
    wclear(win_main);
    wclear(win_info);
    wrefresh(win_main);
    wrefresh(win_info);
    wrefresh(win_list);
}

void print_border(WINDOW *win_info, WINDOW *win_main, WINDOW *win_list) {
    box(win_main, 0, 0);
    box(win_info, 0, 0);
    box(win_list, 0, 0);
}

void input(WINDOW *win_info, WINDOW *win_main, WINDOW *win_list, struct book **books, char *username) {
    char change_input[256] = "";
    getstr(change_input);
    switch (selected_edit_row) {
        case 0:
            memcpy(edit_book.title, change_input, 256);
            break;
        case 1:
            memcpy(edit_book.authors, change_input, 256);
            break;
        case 2:
            memcpy(edit_book.annotation, change_input, 256);
            break;
        case 3:
            memcpy(edit_book.tags, change_input, 256);
            break;
    }
    active_tab = EDIT_MODE;
    update_view(win_info, win_main, win_list, books, username);
}


void print_menu(WINDOW *win_main, struct book **books, char *username) {
    if (active_tab == CHOOSE_MODE) {
        mvwprintw(win_main, 0, 0, "[w-up]");
        mvwprintw(win_main, 0, 10, "[e-down]");
        mvwprintw(win_main, 0, 20, "[s-select book]");
        mvwprintw(win_main, 0, 35, "[a-add book]");
        mvwprintw(win_main, 0, 55, "[t-search]");
    } else if (active_tab == SEARCH_MODE) {
        mvwprintw(win_main, 0, 0, "[b-back]");
        mvwprintw(win_main, 0, 10, "[s-search]");
    } else if (active_tab == ACTIVE_MODE) {
        mvwprintw(win_main, 0, 0, "[b-back]");
        mvwprintw(win_main, 0, 10, "[e-edit]");
        if (books[selected_book]->available == 1) {
            mvwprintw(win_main, 0, 20, "[g-get book]");
        } else if (books[selected_book]->available == 0 && strcmp(books[selected_book]->get_client, username) == 0) {
            mvwprintw(win_main, 0, 20, "[g-return book]");
        }
    } else if (active_tab == EDIT_MODE) {
        mvwprintw(win_main, 0, 0, "[b-back]");
        mvwprintw(win_main, 0, 10, "[s-save]");
        mvwprintw(win_main, 0, 20, "[c-change]");
    }
    mvwprintw(win_main, 0, 90, "[q-exit]");
    curs_set(0);
}

void print_book(WINDOW *win_list, struct book **books) {
    int length = get_books_length(books);
    int y = 1;
    int x = 2;
    char *p, text[126];
    p = text;
    for (int i = 0; i < length; i++) {
        if (selected_book == i && active_tab == 0) wattron(win_list, A_STANDOUT);
        sprintf(text, "%d", books[i]->id);
        mvwprintw(win_list, y, x, p);
        x += 2;
        mvwprintw(win_list, y, x, books[i]->title);
        if (selected_book == i && active_tab == 0) wattroff(win_list, A_STANDOUT);
        y += 2;
        x = 2;
    }
}

void show_book_info(WINDOW *win_info, struct book **books) {
    int y = 1;
    int x = 2;
    char *p, text[126];
    p = text;
    sprintf(text, "%d", books[selected_book]->id);
    char *available;
    if (books[selected_book]->available == 1) {
        available = "yes";
    } else {
        available = "no";
    }
    print_row("ID:", p, &x, &y, win_info);
    print_row("Title:", books[selected_book]->title, &x, &y, win_info);
    print_row("Author:", books[selected_book]->authors, &x, &y, win_info);
    print_row("Annotation:", books[selected_book]->annotation, &x, &y, win_info);
    print_row("Tags:", books[selected_book]->tags, &x, &y, win_info);
    print_row("Available:", available, &x, &y, win_info);
}

void edit_book_info(WINDOW *win_info, struct book book) {
    int y = 1;
    int x = 2;
    edit_book = book;
    if (active_tab == EDIT_MODE && selected_edit_row == 0)wattron(win_info, A_STANDOUT);
    print_row("Title:", edit_book.title, &x, &y, win_info);
    if (active_tab == EDIT_MODE && selected_edit_row == 0)wattroff(win_info, A_STANDOUT);
    if (active_tab == EDIT_MODE && selected_edit_row == 1)wattron(win_info, A_STANDOUT);
    print_row("Author:", edit_book.authors, &x, &y, win_info);
    if (active_tab == EDIT_MODE && selected_edit_row == 1)wattroff(win_info, A_STANDOUT);
    if (active_tab == EDIT_MODE && selected_edit_row == 2)wattron(win_info, A_STANDOUT);
    print_row("Annotation:", edit_book.annotation, &x, &y, win_info);
    if (active_tab == EDIT_MODE && selected_edit_row == 2)wattroff(win_info, A_STANDOUT);
    if (active_tab == EDIT_MODE && selected_edit_row == 3)wattron(win_info, A_STANDOUT);
    print_row("Tags:", edit_book.tags, &x, &y, win_info);
    if (active_tab == EDIT_MODE && selected_edit_row == 3)wattroff(win_info, A_STANDOUT);
}

void input_search(WINDOW *win_info, WINDOW *win_main, WINDOW *win_list, struct book **books, char *username) {
    char change_input[255] = "";
    getstr(change_input);
    memcpy(search_input, change_input, 255);

    int book_length = get_books_length(books);
    int founded_index = 0;
    struct book **arr_books = malloc(founded_index * sizeof(struct book *));
    for (int i = 0; i < book_length; i++) {
        if (check_contain(books[i]) == 0) {
            arr_books = realloc(arr_books, (founded_index + 1) * (sizeof(struct book)));
            arr_books[founded_index] = malloc(sizeof(struct book));
            arr_books[founded_index]->id = books[i]->id;
            strcpy(arr_books[founded_index]->title, books[i]->title);
            founded_index++;
        }
    }
    arr_books[founded_index] = NULL;

    update_view(win_info, win_main, win_list, arr_books, username);
}

int check_contain(struct book *book) {
    char *str = "";
    char *str2 = search_input;
    switch (search_tab) {
        case 0:
            str = book->title;
            break;
        case 1:
            str = book->authors;
            break;
        case 2:
            str = book->annotation;
            break;
        case 3:
            str = book->tags;
            break;
    }
    char *ret = strcasestr(str, str2);
    if (ret) return 0;
    return 1;
};

void search(WINDOW *win_info) {
    int y = 1;
    int x = 2;
    mvwprintw(win_info, y, x, "Select a search field:");
    y += 2;

    if (search_tab == 0)wattron(win_info, A_STANDOUT);
    mvwprintw(win_info, y, x, "Title");
    y += 2;
    if (search_tab == 0)wattroff(win_info, A_STANDOUT);

    if (search_tab == 1)wattron(win_info, A_STANDOUT);
    mvwprintw(win_info, y, x, "Author");
    y += 2;
    if (search_tab == 1)wattroff(win_info, A_STANDOUT);

    if (search_tab == 2)wattron(win_info, A_STANDOUT);
    mvwprintw(win_info, y, x, "Annotation");
    y += 2;
    if (search_tab == 2)wattroff(win_info, A_STANDOUT);

    if (search_tab == 3)wattron(win_info, A_STANDOUT);
    mvwprintw(win_info, y, x, "Tags");
    y += 2;
    if (search_tab == 3)wattroff(win_info, A_STANDOUT);
    mvwprintw(win_info, y, x, "Value: ");
    x += 7;
    mvwprintw(win_info, y, x, search_input);
}

void print_row(char *call_row, char *row, int *x, int *y, WINDOW *win_info) {
    mvwprintw(win_info, *y, *x, call_row);
    *x += 14;
    mvwprintw(win_info, *y, *x, row);
    *x = 2;
    *y += 2;
}