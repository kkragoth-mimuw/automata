cmake_minimum_required(VERSION 3.9)
project(automata C)

set(CMAKE_C_STANDARD 11)

add_executable(validator validator.c err.h err.c common.h protocol.h spawner.h)
add_executable(run run.c err.h err.c common.h protocol.h spawner.h)
add_executable(tester tester.c err.h err.c common.h protocol.h)
