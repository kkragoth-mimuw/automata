//
// Created by PSzulc on 12/01/2018.
//
#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <mqueue.h>

#include "common.h"
#include "protocol.h"
#include "spawner.h"
#include "err.h"

int snt, rcd, acc;

struct {
    long pid;
    mqd_t mq_desc;
    int p;
    int q;
} testers[MAX_TESTERS];

int main() {
    int N, A, Q, U, F, initial_state;

    bool accepting_states[100];

    int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES];

    char buffer[BUF_SIZE] = { 0 };
    char pid[BUF_SIZE] = { 0 };
    char message[BUF_SIZE] = { 0 };
    char tester_mq_name[BUF_SIZE] = { 0 };

    read_automata_description_from_stdin(&N, &A, &Q, &U, &F, &initial_state, accepting_states, transitions);

    for (int i = 0; i < MAX_TESTERS; i++) {
        testers[i].pid = 0;
    }

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = BUF_SIZE;
    attr.mq_curmsgs = 0;

    mqd_t validator_mq_desc = mq_open(VALIDATOR_MQ, O_RDWR | O_CREAT, 0644, &attr);

    if (validator_mq_desc == (mqd_t) - 1) {
        syserr("Error in mq_open");
    }


    int running_children = 0;
    bool accept_new_words = true;

    while(accept_new_words || (running_children > 0)) {
        memset(buffer, 0, BUF_SIZE);
        memset(message, 0, BUF_SIZE);
        memset(tester_mq_name, 0, BUF_SIZE);

        if (mq_receive(validator_mq_desc, buffer, BUF_SIZE, NULL) < 0) {
            syserr("Error in rec: ");
       }

        strcat(message, buffer + strcspn(buffer, "#") + 1);
        buffer[strcspn(buffer, "#")] = 0;

        printf("VALIDATOR: %s\n%s\n***\n", buffer, message);

        long pid = atol(buffer);
        int tester_position = 0;
        for (; tester_position < MAX_TESTERS; tester_position++) {
            if (testers[tester_position].pid == pid) {
                break;
            }
            if (testers[tester_position].pid == 0) {
                testers[tester_position].pid = pid;
                sprintf(tester_mq_name, "%s%s",TESTER_MQ_PREFIX,  buffer);
                testers[tester_position].mq_desc = mq_open(tester_mq_name, O_WRONLY);

                if (testers[tester_position].mq_desc == (mqd_t) -1) {
                    syserr("Error in mq_open");
                }

                testers[tester_position].p = 0;
                testers[tester_position].q = 0;
                break;
            }
        }

        if (strcmp(message, ENDING_SYMBOL) == 0) {
            accept_new_words = false;
        }

        if (strcmp(message, WORD_IS_VALID) == 0) {
            acc += 1;
            testers[tester_position].q += 1;
        }

        if (strcmp(message, WORD_IS_INVALID) == 0 || strcmp(message, WORD_IS_VALID) == 0 || strcmp(message, ENDING_SYMBOL) == 0) {
            if (strcmp(message, ENDING_SYMBOL) != 0) {
                running_children -= 1;
                snt += 1;
                if (wait(NULL) == -1) syserr("Error in wait\n");
            }
            int ret = mq_send(testers[tester_position].mq_desc, message, strlen(message), 1);
            if (ret) {
                syserr("Error in mq_send");
            }
        }
        else if (accept_new_words) {
            rcd += 1;
            testers[tester_position].p += 1;
            strcat(message, END_OF_WORD);
            spawn_root_run(&N, &A, &Q, &U, &F, &initial_state, accepting_states, transitions, buffer, message, initial_state);
            running_children += 1;
        }
    }

    printf("Rcd: %d\nSnt: %d\nAcc: %d\n", rcd, snt, acc);

    for (int i = 0; i < MAX_TESTERS; i++) {
        if (testers[i].pid == 0) {
            break;
        }

        // killuj testera

        printf("PID: %ld\nRcd: %d\nAcc: %d\n", testers[i].pid, testers[i].p, testers[i].q);
    }

    if (mq_close(validator_mq_desc)) {
        syserr("Error in close");
    }
    if (mq_unlink(VALIDATOR_MQ)) {
        syserr("Error in unlink");
    }
}

