cmake_minimum_required(VERSION 3.16)
project(snake C)

add_executable(snake snake.c)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

target_compile_options(snake PRIVATE ${SDL2_CFLAGS})
target_link_libraries(snake PRIVATE ${SDL2_LIBRARIES})
