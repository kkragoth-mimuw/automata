#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "err.h"
#include "protocol.h"
#include "common.h"
#include "spawner.h"

int main() {
    int N, A, Q, U, F, initial_state, current_state;
    bool accepting_states[100];
    int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES];

    char pid[BUF_SIZE] = { 0 };
    char word[BUF_SIZE] = { 0 };
    char message[BUF_SIZE] = { 0 };

    read_automata_description_from_stdin(&N, &A, &Q, &U, &F, &initial_state, accepting_states, transitions);

    scanf("%s %s %d", pid, word, &current_state);

    // ALGORITHM HERE:
    bool is_word_valid = false;
    int children_created = 0;

    if (strcmp(word, "") == 0) {
        is_word_valid = accepting_states[current_state];

        if (is_word_valid) {
            printf("%s\n", WORD_IS_VALID);
        }
        else {
            printf("%s\n", WORD_IS_INVALID);
        }
    }
    else {
        int transition_letter = map_char_to_int(word[0]);

        for (int i = 0; i < MAX_STATES; i++) {
            int next_state = transitions[current_state][transition_letter][i];

            if (next_state == INVALID_STATE) {
                break;
            }

            spawn_run(&N, &A, &Q, &U, &F, &initial_state, accepting_states, transitions, "", word + 1, next_state);

            children_created += 1;
        }
        if (current_state < U) { // universal state

        } else { //existential state

        }
    }

    if (strcmp(pid, INVALID_PID) != 0) {
        mqd_t validator_desc = mq_open(VALIDATOR_MQ, O_WRONLY);

        sprintf(message, "%s#%s", pid, is_word_valid ? WORD_IS_VALID : WORD_IS_INVALID);

        int ret = mq_send(validator_desc, message, strlen(message), 1);
        if (ret)
            syserr("Error in mq_send");
    }
}
