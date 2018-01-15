#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "common.h"
#include "err.h"

void spawn_run() {
    int pipes[2][2];
    char buffer[BUF_SIZE] = "";

    if ((pipe(pipes[0]) == -1) || (pipe(pipes[1]) == -1)) {
        syserr("ERROR: pipe");
    }

    switch((fork())) {
        case -1:
                syserr("ERROR: fork");
        case 0:
                if ((dup2(pipes[1])))
    }
}
