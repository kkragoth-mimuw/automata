cmake_minimum_required(VERSION 3.1)
project(automata C)

set(CMAKE_C_STANDARD 11)

add_executable(validator validator.c err.h err.c common.h common.c protocol.h protocol.c spawner.h spawner.c)
target_link_libraries(validator rt)
add_executable(run run.c err.h err.c common.h common.c protocol.h protocol.c spawner.h spawner.c)
target_link_libraries(run rt)
add_executable(tester tester.c err.h err.c common.h common.c protocol.h protocol.c)
target_link_libraries(tester rt)


