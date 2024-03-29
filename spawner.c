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
        kill(validator_pid, SIGINT);
        syserr("pipe\n");
    }

    switch (fork())
    {
        case -1:
            kill(validator_pid, SIGINT);
            syserr("fork\n");

        case 0:
            if ((dup2(CHILD_READ_FD, STDIN_FILENO) == -1) || (dup2(CHILD_WRITE_FD, STDOUT_FILENO) == -1)) {
                kill(validator_pid, SIGINT);
                syserr("dup2\n");
            }

            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    if (close(pipes[i][j]) == -1) {
                        kill(validator_pid, SIGINT);
                        syserr("close\n");
                    }
                }
            }

            execl("./run", "run", NULL);
            kill(validator_pid, SIGINT);
            syserr("exec\n");

        default:
            if ((close(CHILD_READ_FD) == -1) || (close(CHILD_WRITE_FD) == -1)) {
                kill(validator_pid, SIGINT);
                syserr("close\n");
            }

            print_automata(PARENT_WRITE_FD, N, A, Q, U, F, initial_state, accepting_states, transitions);
            // pass configuration
            if (dprintf(PARENT_WRITE_FD, "%d %s %s %d", validator_pid, pid, word, next_state) < 0) {
                kill(validator_pid, SIGINT);
                syserr("dprint\n");
            }

            if(close(PARENT_WRITE_FD)) {
                kill(validator_pid, SIGINT);
                syserr("close\n");
            }

            return PARENT_READ_FD;
    }
}

void spawn_root_run(pid_t validator_pid, int *N, int *A, int *Q, int *U, int *F, int *initial_state, bool *accepting_states, int transitions[MAX_STATES][ALPHABET_SIZE][MAX_STATES], char *pid, char *word, int next_state) {
    int pipe_dsc[2];
    if (pipe(pipe_dsc) == -1) {
        kill(validator_pid, SIGINT);
        syserr("Error in pipe");
    }

    switch (fork())
    {
        case -1:
            kill(validator_pid, SIGINT);
            syserr("fork\n");

        case 0:
            if (close (0) == -1) {
                kill(validator_pid, SIGINT);
                syserr("Error in child, close (0)");
            }
            if (dup (pipe_dsc [0]) != 0) {
                kill(validator_pid, SIGINT);
                syserr("Error in child, dup (pipe_dsc [0])\n");
            }
            if (close (pipe_dsc [0]) == -1) {
                kill(validator_pid, SIGINT);
                syserr("Error in child, close (pipe_dsc [0])\n");
            }
            if (close (pipe_dsc [1]) == -1) {
                kill(validator_pid, SIGINT);
                syserr("Error in child, close (pipe_dsc [1])\n");
            }

            execl("./run", "run", NULL);
            syserr("exec\n");
            kill(validator_pid, SIGINT);

        default:
            if (close (pipe_dsc [0]) == -1) {
                syserr("Error in parent, close (pipe_dsc [0])\n");
                kill(validator_pid, SIGINT);
            }

            // pass automata description
            print_automata(pipe_dsc[1], N, A, Q, U, F, initial_state, accepting_states, transitions);
            // pass configuration
            if (dprintf(pipe_dsc[1], "%d %s %s %d", validator_pid, pid, word, next_state) < 0) {
                kill(validator_pid, SIGINT);
                syserr("dprintf");
            }

            if (close (pipe_dsc [1]) == -1) {
                kill(validator_pid, SIGINT);
                syserr("Error in parent, close (pipe_dsc [1])\n");
            }

            return;
    }
}
