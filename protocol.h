//
// Created by PSzulc on 14/01/2018.
//

#ifndef AUTOMATA_PROTOCOL_H
#define AUTOMATA_PROTOCOL_H

#include <unistd.h>
#include <sys/types.h>

#define MAX_MSGS 10
#define MAX_TESTERS 1000

const char* VALIDATOR_MQ = "/validatorMq";
const char* TESTER_MQ_PREFIX = "/testerMq";

const char* END_OF_WORD = "$";
const char* WORD_IS_INVALID = "INVALID_WORD";
const char* WORD_IS_VALID = "VALID_WORD";
const char* ENDING_SYMBOL = "!";

char* get_tester_mq_name(pid_t queue_pid);

#endif //AUTOMATA_PROTOCOL_H
