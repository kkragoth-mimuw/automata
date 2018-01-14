//
// Created by PSzulc on 14/01/2018.
//

#ifndef AUTOMATA_COMMON_H
#define AUTOMATA_COMMON_H

const int BUF_SIZE = 256;

const int MAX_STATES = 100;

const char ALPHABET_MIN = 'a';
const char ALPHABET_MAX = 'z';

const int ALPHABET_SIZE = 'z' - 'a' + 1;

const int INVALID_STATE = -1;


int map_char_to_int(char c) { return c - ALPHABET_MIN; }

#endif //AUTOMATA_COMMON_H
