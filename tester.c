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
    char buffer[BUF_SIZE] = { 0 };
    char message[BUF_SIZE] = { 0 };

    struct mq_attr mq_a;

    mqd_t validator_desc = mq_open(VALIDATOR_MQ, O_WRONLY);

    if (validator_desc == (mqd_t) -1) {
        syserr("Error in mq_open");
    }

    if (mq_getattr(validator_desc, &mq_a))
            syserr("Error in getattr");
    int buff_size = mq_a.mq_msgsize + 1;

    while(scanf("%s", buffer) != EOF) {
        memset(message, 0, BUF_SIZE);
        sprintf(message, "%ld#%s", (long) getpid(), buffer);

        int ret = mq_send(validator_desc, message, strlen(message), 1);
        if (ret)
            syserr("Error in mq_send");
    }

//    if (mq_close(validator_desc)) {
//        syserr("Error in close");
//    }

//    if (mq_unlink(VALIDATOR_MQ)) {
//        syserr("Error in unlink");
//    }

}
