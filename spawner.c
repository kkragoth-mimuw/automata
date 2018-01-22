#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include "common.h"
#include "err.h"

#include "spawner.h"

int spawn_run(pid_t validator_pid, int *N, int *A, int *Q, int *U, int *F, int *initial_state, bool *accepting_states, int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES], char *pid, char *word, int next_state) {
    int pipes[2][2];
    char buffer[BUF_SIZE] = "";

    if ((pipe(PARENT_READ_PIPE) == -1) ||(pipe(PARENT_WRITE_PIPE) == -1)) {
        syserr("pipe\n");
        kill(validator_pid, SIGINT);
    }

    switch (fork())
    {
        case -1:
            syserr("fork\n");
            kill(validator_pid, SIGINT);

        case 0:
            if ((dup2(CHILD_READ_FD, STDIN_FILENO) == -1) || (dup2(CHILD_WRITE_FD, STDOUT_FILENO) == -1)) {
                syserr("dup2\n");
                kill(validator_pid, SIGINT);
            }

            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    if (close(pipes[i][j]) == -1) {
                        syserr("close\n");
                        kill(validator_pid, SIGINT);
                    }
                }
            }

            execl("./run", "run", NULL);
            syserr("exec\n");
            kill(validator_pid, SIGINT);

        default:
            if ((close(CHILD_READ_FD) == -1) || (close(CHILD_WRITE_FD) == -1)) {
                syserr("close\n");
                kill(validator_pid, SIGINT);
            }

            print_automata(PARENT_WRITE_FD, N, A, Q, U, F, initial_state, accepting_states, transitions);
            // pass configuration
            dprintf(PARENT_WRITE_FD, "%d %s %s %d", validator_pid, pid, word, next_state);

            if(close(PARENT_WRITE_FD)) {
                syserr("close\n");
                kill(validator_pid, SIGINT);
            }

            return PARENT_READ_FD;
    }
}

void spawn_root_run(pid_t validator_pid, int *N, int *A, int *Q, int *U, int *F, int *initial_state, bool *accepting_states, int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES], char *pid, char *word, int next_state) {
    int pipe_dsc[2];
    if (pipe(pipe_dsc) == -1) {
        syserr("Error in pipe");
        kill(validator_pid, SIGINT);
    }

    switch (fork())
    {
        case -1:
            syserr("fork\n");
            kill(validator_pid, SIGINT);

        case 0:
            if (close (0) == -1)            syserr("Error in child, close (0)\n");
            if (dup (pipe_dsc [0]) != 0)    syserr("Error in child, dup (pipe_dsc [0])\n");
            if (close (pipe_dsc [0]) == -1) syserr("Error in child, close (pipe_dsc [0])\n");
            if (close (pipe_dsc [1]) == -1) syserr("Error in child, close (pipe_dsc [1])\n");

            execl("./run", "run", NULL);
            syserr("exec\n");
            kill(validator_pid, SIGINT);

        default:
            if (close (pipe_dsc [0]) == -1) syserr("Error in parent, close (pipe_dsc [0])\n");

            // pass automata description
            print_automata(pipe_dsc[1], N, A, Q, U, F, initial_state, accepting_states, transitions);
            // pass configuration
            dprintf(pipe_dsc[1], "%d %s %s %d", validator_pid, pid, word, next_state);

            if (close (pipe_dsc [1]) == -1) syserr("Error in parent, close (pipe_dsc [1])\n");

            return;
    }
}
