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

int snt, rcd, acc;

int main() {
    printf("PID: %d", getpid());

    char buffer[BUF_SIZE] = { 0 };
    char message[BUF_SIZE] = { 0 };
    char my_mq_name[BUF_SIZE] = { 0 };

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = BUF_SIZE;
    attr.mq_curmsgs = 0;

    strcat(my_mq_name, TESTER_MQ_PREFIX);
    sprintf(buffer, "%ld", (long)getpid());
    strcat(my_mq_name, buffer);
    mqd_t my_mq_desc = mq_open(my_mq_name, O_RDWR | O_CREAT, 0644, &attr);
    if (my_mq_desc == (mqd_t) -1) {
        syserr("Error in mq_open");
    }

    mqd_t validator_desc = mq_open(VALIDATOR_MQ, O_WRONLY);

    if (validator_desc == (mqd_t) -1) {
        syserr("Error in mq_open");
    }

    while(scanf("%s", buffer) != EOF) {
        memset(message, 0, BUF_SIZE);
        sprintf(message, "%ld#%s", (long) getpid(), buffer);

        int ret = mq_send(validator_desc, message, strlen(message), 1);
        if (ret)
            syserr("Error in mq_send");

        if (strcmp(buffer, ENDING_SYMBOL) != 0) {
            snt += 1;
        }

        if (mq_receive(my_mq_desc, buffer, BUF_SIZE, NULL) < 0) {
             syserr("Error in rec: ");
        }

        if (strcmp(buffer, ENDING_SYMBOL) != 0) {
            rcd += 1;
        }

        if (strcmp(buffer, WORD_IS_VALID) == 0) {
            acc += 1;
        }
    }

    printf("PID: %d\nSnt: %d\nRcd: %d\nAcc: %d\n", getpid(), snt, rcd, acc);

    if (mq_close(my_mq_desc)) {
        syserr("Error in close");
    }

    if (mq_unlink(my_mq_name)) {
        syserr("Error in unlink");
    }
}
