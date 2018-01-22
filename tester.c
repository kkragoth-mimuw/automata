//
// Created by PSzulc on 15/01/2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "protocol.h"
#include "common.h"
#include "err.h"

mqd_t my_mq_desc;
char my_mq_name[BUF_SIZE] = { 0 };

int snt, rcd, acc;

void print_stats() {
    printf("Snt: %d\nRcd: %d\nAcc: %d\n", snt, rcd, acc);
}

void panic_cleanup() {
    mq_close(my_mq_desc);
    mq_unlink(my_mq_name);
}

int main() {
    printf("PID: %d\n", getpid());

    char buffer[BUF_SIZE] = { 0 };
    char message[BUF_SIZE] = { 0 };

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = BUF_SIZE;
    attr.mq_curmsgs = 0;

    strcat(my_mq_name, TESTER_MQ_PREFIX);
    sprintf(buffer, "%ld", (long)getpid());
    strcat(my_mq_name, buffer);
    my_mq_desc = mq_open(my_mq_name, O_RDWR | O_CREAT, 0644, &attr);
    if (my_mq_desc == (mqd_t) -1) {
        syserr("Error in mq_open");
    }

    mqd_t validator_desc = mq_open(VALIDATOR_MQ, O_WRONLY);

    if (validator_desc == (mqd_t) -1) {
        syserr("Error in mq_open");
    }

    bool finished = false;

    while(!finished && fgets(buffer, BUF_SIZE, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
        memset(message, 0, BUF_SIZE);
        sprintf(message, "%ld#%s", (long) getpid(), buffer);

        int ret = mq_send(validator_desc, message, strlen(message), 1);
        if (ret)
            syserr("Error in mq_send");

        if (strcmp(buffer, ENDING_SYMBOL) != 0) {
            snt += 1;
            printf("%s ", buffer);
        }

        memset(buffer, 0, BUF_SIZE);

        if (mq_receive(my_mq_desc, buffer, BUF_SIZE, NULL) < 0) {
             syserr("Error in rec: ");
        }

        if (strcmp(buffer, WORD_IS_VALID) == 0) {
            acc += 1;
            rcd += 1;
            printf("A\n");
        }

        if (strcmp(buffer, WORD_IS_INVALID) == 0) {
            rcd += 1;
            printf("N\n");
        }

        if (strcmp(buffer, ENDING_SYMBOL) == 0) {
            finished = true;
        }
    }

    print_stats();

    if (mq_close(my_mq_desc)) {
        syserr("Error in close");
    }

    if (mq_unlink(my_mq_name)) {
        syserr("Error in unlink");
    }
}
