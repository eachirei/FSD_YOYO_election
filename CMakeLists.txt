cmake_minimum_required(VERSION 3.12)
project(FSD C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_COMPILER /usr/local/bin/mpicc)

add_executable(FSD main.c source.c source.h utils.c utils.h interm.c interm.h sink.c sink.h)