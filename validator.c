//
// Created by PSzulc on 12/01/2018.
//
#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <string.h>
#include <mqueue.h>

#include "common.h"

char buffer[BUF_SIZE] = { 0 };
size_t buf_size = BUF_SIZE;


// Automata description
int N, A, Q, U, F, initial_state;

bool accepting_states[100];

int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES];
// End of Automata


void initialize_accepting_states_from_stdin() {
    int accepting_state;
    fgets(&buffer, BUF_SIZE - 2, stdin);

    int bytes_consumed = 0;
    int bytes_now = 0;

    while (sscanf(buffer + bytes_consumed, "%d%n", &accepting_state, &bytes_now) != -1) {
        accepting_states[accepting_state] = true;
        printf("%d\n", accepting_state);
        bytes_consumed += bytes_now;
    }
}


void initialize_transitions_from_stdin(int lines) {
    memset(transitions, INVALID_STATE, MAX_STATES * ALPHABET_SIZE * MAX_STATES);

    while (lines--) {
        char letter;
        int state_from, state_to;
        int number_of_states_to_already_processed = 0;

        fgets(&buffer, BUF_SIZE - 2, stdin);

        int bytes_now = 0;

        sscanf(buffer, "%d %c%n", &state_to, &letter, &bytes_now);

        int bytes_consumed = bytes_consumed;

        int transition_letter = map_char_to_int(letter);

        while (sscanf(buffer + bytes_consumed, "%d%n", &state_to, &bytes_now) != -1) {
            transitions[state_from][transition_letter][number_of_states_to_already_processed] = state_to;
            bytes_consumed += bytes_now;
            number_of_states_to_already_processed++;
        }
    }
}


void read_automata_description_from_stdin() {
    scanf("%d %d %d %d %d\n", &N, &A, &Q, &U, &F);
    scanf("%d\n", &initial_state);

    initialize_accepting_states_from_stdin();

    initialize_transitions_from_stdin(N - 3);
}


int main() {
    read_automata_description_from_stdin();
    while(1) {

    }
}
