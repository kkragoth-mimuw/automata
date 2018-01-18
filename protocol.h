//
// Created by PSzulc on 14/01/2018.
//

#ifndef AUTOMATA_PROTOCOL_H
#define AUTOMATA_PROTOCOL_H

const char* VALIDATOR_MQ_NAME = "validator_mq";

const char* TESTER_MQ_PREFIX = "tester_mq_";

const char* WORD_IS_INVALID = "INVALID_WORD";
const char* WORD_IS_VALID = "VALID_WORD";
const char* ENDING_SYMBOL = "!";

char* get_tester_mq_name(pid_t queue_pid) {

}

#endif //AUTOMATA_PROTOCOL_H
