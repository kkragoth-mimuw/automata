//
// Created by PSzulc on 14/01/2018.
//

#ifndef AUTOMATA_COMMON_H
#define AUTOMATA_COMMON_H

#include <stdbool.h>

#define BUF_SIZE 8192
#define MAX_STATES  100
#define ALPHABET_SIZE 'z' - 'a' + 1

#define INVALID_STATE -1

int map_char_to_int(char c);
char map_int_to_Char(int c);

void initialize_accepting_states_from_stdin(bool *accepting_states);
void print_accepting_states(int fd, bool *accepting_states);

void initialize_transitions_from_stdin(int lines, int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES]);
void print_transitions(int fd, int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES]);

void read_automata_description_from_stdin(int *N, int *A, int *Q, int *U, int *F, int *initial_state, bool *accepting_states, int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES]);
void print_automata(int fd, int *N, int *A, int *Q, int *U, int *F, int *initial_state, bool *accepting_states, int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES]);


#endif //AUTOMATA_COMMON_H
