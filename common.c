#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "common.h"

int map_char_to_int(char c) { return c - 'a'; }
char map_int_to_char(int c) { return c + 'a'; }

void print_accepting_states(int fd, bool *accepting_states) {
    for (int i = 0; i < 100; i++) {
        if (accepting_states[i]) {
            dprintf(fd, "%d ", i);
        }
    }

    dprintf(fd, "\n");
}

void read_automata_description_from_stdin(int *N, int *A, int *Q, int *U, int *F, int *initial_state, bool *accepting_states, int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES]) {
    scanf("%d %d %d %d %d\n", N, A, Q, U, F);
    scanf("%d\n", initial_state);

    initialize_accepting_states_from_stdin(accepting_states);

    initialize_transitions_from_stdin((*N) - 3, transitions);
}

void print_automata(int fd, int *N, int *A, int *Q, int *U, int *F, int *initial_state, bool *accepting_states, int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES]) {
    dprintf(fd, "%d %d %d %d %d\n", *N, *A, *Q, *U, *F);

    dprintf(fd, "%d\n", *initial_state);

    print_accepting_states(fd, accepting_states);

    print_transitions(fd, transitions);
}

void initialize_transitions_from_stdin(int lines, int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES]) {
    char buffer[BUF_SIZE] = { 0 };

    for (int i = 0; i < MAX_STATES; i++) {
        for (int j  = 0; j < ALPHABET_SIZE; j++) {
            for (int k = 0; k < ALPHABET_SIZE; k++) {
                transitions[i][j][k] = INVALID_STATE;
            }
        }
    }

    while (lines--) {
        char letter;
        int state_from, state_to;
        int number_of_states_to_already_processed = 0;

        fgets(buffer, BUF_SIZE - 2, stdin);

        int bytes_now = 0;

        sscanf(buffer, "%d %c%n", &state_from, &letter, &bytes_now);

        int bytes_consumed = bytes_now;

        int transition_letter = map_char_to_int(letter);

        while (sscanf(buffer + bytes_consumed, "%d%n", &state_to, &bytes_now) != -1) {
            transitions[state_from][transition_letter][number_of_states_to_already_processed] = state_to;
            bytes_consumed += bytes_now;
            number_of_states_to_already_processed++;
        }
    }
}

void print_transitions(int fd, int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES]) {
    for (int i = 0; i < MAX_STATES; i++) {
        for (int j = 0; j < ALPHABET_SIZE; j++) {
            if (transitions[i][j][0] != INVALID_STATE) {
                dprintf(fd, "%d %c ", i, map_int_to_char(j));

                for (int k = 0; k < MAX_STATES; k++) {
                    if (transitions[i][j][k] != INVALID_STATE) {
                        dprintf(fd, "%d ", transitions[i][j][k]);
                    }
                    else {
                        break;
                    }
                }

                dprintf(fd, "\n");
            }
        }
    }
}

void initialize_accepting_states_from_stdin(bool *accepting_states) {
    char buffer[BUF_SIZE] = { 0 };

    for (int i = 0; i < MAX_STATES; i++) {
        accepting_states[i] = false;
    }

    int accepting_state;
    fgets(buffer, BUF_SIZE - 2, stdin);

    int bytes_consumed = 0;
    int bytes_now = 0;

    while (sscanf(buffer + bytes_consumed, "%d%n", &accepting_state, &bytes_now) != -1) {
        accepting_states[accepting_state] = true;
        bytes_consumed += bytes_now;
    }
}
