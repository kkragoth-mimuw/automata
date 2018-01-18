#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "err.h"
#include "protocol.h"
#include "common.h"
#include "spawner.h"

// Automata description
int N, A, Q, U, F, initial_state;

bool accepting_states[100];

int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES];

size_t buf_size = BUF_SIZE;
char buffer[BUF_SIZE] = { 0 };

size_t buf_size = BUF_SIZE;
char word[BUF_SIZE] = { 0 };

int main() {
    scanf("%d %d %d %d %d\n", &N, &A, &Q, &U, &F);
    scanf("%s", word)

}
