//
// Created by PSzulc on 15/01/2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <mqueue.h>

#include "protocol.h"
#include "common.h"
#include "err.h"

mqd_t validator_desc, my_mq_desc;
char my_mq_name[BUF_SIZE] = { 0 };

bool finished = false;

int snt, rcd, acc;

char buffer[BUF_SIZE] = { 0 };
char word[BUF_SIZE] = { 0 } ;

struct sigevent sev;

void print_stats() {
    printf("Snt: %d\nRcd: %d\nAcc: %d\n", snt, rcd, acc);
}

void panic_cleanup() {
    mq_close(my_mq_desc);
    mq_close(validator_desc);
    mq_unlink(my_mq_name);
}

void sigint_handler(int sign) {
    print_stats();
    panic_cleanup();
    exit(0);
}

static void mq_notify_callback(union sigval sv) {
    memset(buffer, 0, BUF_SIZE);
    memset(word, 0, BUF_SIZE);

    struct mq_attr attr;

    bool registered_for_mq_update = false;

    do {
        if (mq_receive(my_mq_desc, buffer, BUF_SIZE, NULL) < 0) {
            panic_cleanup();
            syserr("Error in rec: ");
        }

        if (strstr(buffer, ENDING_SYMBOL) != NULL) {
            finished = true;
        }
        else {
            buffer[strlen(buffer) - 1] = 0; // consume last '$';
            printf("%s ", buffer + strcspn(buffer, "#") + 1);
        }

        if (strstr(buffer, WORD_IS_VALID) != NULL) {
            acc += 1;
            rcd += 1;
            printf("A\n");
        }

        if (strstr(buffer, WORD_IS_INVALID) != NULL) {
            rcd += 1;
            printf("N\n");
        }

        if (finished && (snt == rcd)) {
            print_stats();
            close(0);
            exit(0);
        }

        if (mq_getattr(my_mq_desc, &attr)) {
            panic_cleanup();
            syserr("Error in getattr");
        }

        if (!registered_for_mq_update) {
            if (mq_notify(my_mq_desc, &sev) == -1) {
                panic_cleanup();
                syserr("Error in mq_notify");
            }

            registered_for_mq_update = true;
        }
    } while(attr.mq_curmsgs > 0);
}


int main() {
    struct sigaction action;
    sigset_t block_mask;
    sigemptyset(&block_mask);
    action.sa_handler = sigint_handler;
    action.sa_mask = block_mask;
    action.sa_flags = 0;

    if (sigaction(SIGINT, &action, 0) == -1) {
        syserr("sigaction");
    }

    printf("PID: %d\n", getpid());

    char buffer[BUF_SIZE] = { 0 };
    char message[BUF_SIZE] = { 0 };

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = BUF_SIZE;
    attr.mq_curmsgs = 0;

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = mq_notify_callback;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = & my_mq_desc;

    sprintf(my_mq_name, "%s%ld", TESTER_MQ_PREFIX, (long)getpid());
    my_mq_desc = mq_open(my_mq_name, O_RDWR | O_CREAT, 0644, &attr);
    if (my_mq_desc == (mqd_t) -1) {
        panic_cleanup();
        syserr("Error in mq_open");
    }

    if (mq_notify(my_mq_desc, &sev) == -1) {
        panic_cleanup();
        syserr("Error in mq_notify");
    }
    validator_desc = mq_open(VALIDATOR_MQ, O_WRONLY);

    if (validator_desc == (mqd_t) -1) {
        panic_cleanup();
        syserr("Error in mq_open");
    }

    memset(message, 0, BUF_SIZE);
    sprintf(message, "%ld#%s", (long) getpid(), INTRODUCTION);
    if (mq_send(validator_desc, message, strlen(message), 1)) {
        panic_cleanup();
        syserr("Error in mq_send");
    }

    while(!finished && fgets(buffer, BUF_SIZE, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
        memset(message, 0, BUF_SIZE);
        sprintf(message, "%ld#%s", (long) getpid(), buffer);

        if (mq_send(validator_desc, message, strlen(message), 1)) {
            panic_cleanup();
            syserr("Error in mq_send");
        }

        if (strcmp(buffer, ENDING_SYMBOL) != 0) {
            snt += 1;
        }
    }

    print_stats();

    if (mq_close(my_mq_desc)) {
        syserr("Error in close");
    }

    if (mq_close(validator_desc)) {
        syserr("Error in close");
    }

    if (mq_unlink(my_mq_name)) {
        syserr("Error in unlink");
    }
}
