#include <stdio.h>
#include <stdlib.h>

#define PARENT_READ_PIPE    pipes[0]
#define PARENT_WRITE_PIPE   pipes[1]
#define PARENT_READ_FD      PARENT_READ_PIPE[0]
#define PARENT_WRITE_FD     PARENT_WRITE_PIPE[1]
#define CHILD_READ_FD       PARENT_WRITE_PIPE[0]
#define CHILD_WRITE_FD      PARENT_READ_PIPE[1]

void spawn_run(int *N, int *A, int *Q, int *U, int *F, int *initial_state, bool *accepting_states, int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES], char *pid, char *word, int next_state);
void spawn_root_run(int *N, int *A, int *Q, int *U, int *F, int *initial_state, bool *accepting_states, int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES], char *pid, char *word, int next_state);
