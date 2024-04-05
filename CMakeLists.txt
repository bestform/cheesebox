cmake_minimum_required(VERSION 3.27)
project(cheesebox C)

set(CMAKE_C_STANDARD 23)

include_directories(
        /usr/include
        /usr/local/include
)

link_directories(
        /usr/lib
        /usr/local/lib
)

link_libraries(
        raylib.a
        dl
        pthread
        m
)

add_executable(cheesebox main.c
        lib/grid.c
        lib/grid.h
)
