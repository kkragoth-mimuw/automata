//
// Created by PSzulc on 12/01/2018.
//
#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "common.h"
#include "protocol.h"
#include "err.h"

int snt, rcd, acc;

int main() {
    int N, A, Q, U, F, initial_state;

    bool accepting_states[100];

    int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES];

    char buffer[BUF_SIZE] = { 0 };

    //read_automata_description_from_stdin(&N, &A, &Q, &U, &F, &initial_state, accepting_states, transitions);
    //print_automata(1, &N, &A, &Q, &U, &F, &initial_state, accepting_states, transitions);

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = BUF_SIZE;
    attr.mq_curmsgs = 0;

    mqd_t validator_mq_desc = mq_open(VALIDATOR_MQ, O_RDWR | O_CREAT, 0644, &attr);

    if (validator_mq_desc == (mqd_t) - 1) {
        syserr("Error in mq_open");
    }

    int received = 0;
    int sent = 0;
    int accepted = 0;

    while(true) {
        if (mq_receive(validator_mq_desc, buffer, BUF_SIZE, NULL) < 0) {
            syserr("Error in rec: ");
       }

        printf("%s\n", buffer);

        if (strcmp(buffer, WORD_IS_VALID) == 0) {
            accepted += 1;
          //send valid/            sent += 1;
        }

        else if (strcmp(buffer, WORD_IS_INVALID) == 0) {
            // send invalid
        }

        else {

        }
    }
}
