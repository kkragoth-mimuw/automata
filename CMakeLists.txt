cmake_minimum_required(VERSION 3.9)
project(automata C)

set(CMAKE_C_STANDARD 11)

add_executable(validator validator.c common.h protocol.h)