//
// Created by PSzulc on 15/01/2018.
//

#include <stdio.h>
#include <mqueue.h>

#include "protocol.h"
#include "common.h"

char buffer[BUF_SIZE] = { 0 };

int main() {
    struct mq_attr mq_a;

    mqd_t desc = mq_open(MSG_QUEUE_NAME, O_RDONLY);

    while(scanf("%s", buffer) != EOF) {

    }

}