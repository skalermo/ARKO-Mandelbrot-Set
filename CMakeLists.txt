cmake_minimum_required(VERSION 3.13)
project(Mandelbrot C)

set(CMAKE_C_STANDARD 99)

add_executable(Mandelbrot main.c)
INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(Mandelbrot ${SDL2_LIBRARIES})