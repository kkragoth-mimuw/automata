//
// Created by PSzulc on 12/01/2018.
//
#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <mqueue.h>

#include "common.h"
#include "protocol.h"
#include "spawner.h"
#include "err.h"

mqd_t validator_mq_desc;

int snt, rcd, acc;

struct {
    long pid;
    mqd_t mq_desc;
    int p;
    int q;
} testers[MAX_TESTERS];


void panic_cleanup() {
    mq_unlink(VALIDATOR_MQ);
    mq_close(validator_mq_desc);

    for (int i = 0; i < MAX_TESTERS; i++) {
        if (testers[i].pid == 0) {
            break;
        }
        kill((pid_t) testers[i].pid, SIGINT);
    }
}

void sigint_handler(int sig) {
    panic_cleanup();
    exit(-1);
}

int main() {
    struct sigaction action;
    sigset_t block_mask;
    sigemptyset(&block_mask);
    action.sa_handler = sigint_handler;
    action.sa_mask = block_mask;
    action.sa_flags = 0;

    if (sigaction (SIGINT, &action, 0) == -1)
        syserr("sigaction");

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MSGS;
    attr.mq_msgsize = BUF_SIZE;
    attr.mq_curmsgs = 0;

    validator_mq_desc = mq_open(VALIDATOR_MQ, O_RDWR | O_CREAT, 0644, &attr);

    if (validator_mq_desc == (mqd_t) - 1) {
        syserr("Error in mq_open");
    }

    int N, A, Q, U, F, initial_state;

    bool accepting_states[100];

    int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES];

    char buffer[BUF_SIZE] = { 0 };
    char pid[BUF_SIZE] = { 0 };
    char message[BUF_SIZE] = { 0 };
    char is_word_valid[BUF_SIZE] = { 0 };
    char tester_mq_name[BUF_SIZE] = { 0 };

    read_automata_description_from_stdin(&N, &A, &Q, &U, &F, &initial_state, accepting_states, transitions);

    for (int i = 0; i < MAX_TESTERS; i++) {
        testers[i].pid = 0;
    }

    int running_children = 0;
    bool accept_new_words = true;

    while(accept_new_words || (running_children > 0)) {
        memset(buffer, 0, BUF_SIZE);
        memset(message, 0, BUF_SIZE);
        memset(tester_mq_name, 0, BUF_SIZE);

        if (mq_receive(validator_mq_desc, buffer, BUF_SIZE, NULL) < 0) {
            panic_cleanup();
            syserr("Error in rec: ");
       }

        strcat(message, buffer + strcspn(buffer, "#") + 1);
        buffer[strcspn(buffer, "#")] = 0;

        long pid = atol(buffer);
        int tester_position = -1;
        if ((strstr(message, INTRODUCTION) != NULL) && accept_new_words) {
            for (int i = 0; i < MAX_TESTERS; i++) {
                if (testers[i].pid == 0) {
                    testers[i].pid = pid;
                    sprintf(tester_mq_name, "%s%s",TESTER_MQ_PREFIX,  buffer);
                    testers[i].mq_desc = mq_open(tester_mq_name, O_WRONLY);

                    if (testers[i].mq_desc == (mqd_t) -1) {
                        syserr("Error in mq_open");
                    }

                    testers[i].p = 0;
                    testers[i].q = 0;

                    tester_position = i;

                    break;
                }
            }
        }

        if (tester_position == -1) {
            for (int i = 0; i < MAX_TESTERS; i++) {
                if (testers[i].pid == pid) {
                    tester_position = i;
                    break;
                }
            }

            if (tester_position == -1) {
                continue;
            }
        }

        if (strstr(message, ENDING_SYMBOL) != NULL) {
            accept_new_words = false;
        }

        else if (strstr(message, WORD_IS_INVALID) != NULL || strstr(message, WORD_IS_VALID) != NULL) {
            if (strstr(message, WORD_IS_VALID) != NULL) {
                acc += 1;
                testers[tester_position].q += 1;
            }
            running_children -= 1;
            snt += 1;
            if (wait(NULL) == -1) {
                panic_cleanup();
                syserr("Error in wait\n");
            }

            if(mq_send(testers[tester_position].mq_desc, message, strlen(message), 1)) {
                panic_cleanup();
                syserr("Error in mq_send");
            }
        }
        else if (accept_new_words && strstr(message, INTRODUCTION) == NULL) {
            rcd += 1;
            testers[tester_position].p += 1;
            strcat(message, END_OF_WORD);
            spawn_root_run(getpid(), &N, &A, &Q, &U, &F, &initial_state, accepting_states, transitions, buffer, message, initial_state);
            running_children += 1;
        }
    }

    printf("Rcd: %d\nSnt: %d\nAcc: %d\n", rcd, snt, acc);

    for (int i = 0; i < MAX_TESTERS; i++) {
        if (testers[i].pid == 0) {
            break;
        }

        mq_send(testers[i].mq_desc, ENDING_SYMBOL, strlen(ENDING_SYMBOL), 1); // dont check; some testers might have already gone away

        printf("PID: %ld\nRcd: %d\nAcc: %d\n", testers[i].pid, testers[i].p, testers[i].q);
    }

    if (mq_close(validator_mq_desc)) {
        syserr("Error in close");
    }
    if (mq_unlink(VALIDATOR_MQ)) {
        syserr("Error in unlink");
    }
}

