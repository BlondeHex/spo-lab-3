cmake_minimum_required(VERSION 3.10)
project(lab3 VERSION 1.0)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILE main.c server/server.c server/server.h main.h client/client.h client/client.c client/utils.c client/utils.h common/network.c common/network.h server/utils.c server/utils.h common/books.h client/ui.c client/ui.h common/books.c)
add_executable(lab3 ${SOURCE_FILE})

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)
find_package(Threads REQUIRED)
if (CURSES_FOUND AND CURSES_HAVE_NCURSES_H)
    include_directories(${CURSES_INCLUDE_DIR})
    target_link_libraries(lab3 PRIVATE Threads::Threads ${CURSES_LIBRARIES} form)
endif ()

