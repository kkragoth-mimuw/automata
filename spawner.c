#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "common.h"
#include "err.h"

#include "spawner.h"

void spawn_run() {
    int pipes[2][2];
    char buffer[BUF_SIZE] = "";

    if ((pipe(PARENT_READ_PIPE) == -1) ||
        (pipe(PARENT_WRITE_PIPE) == -1)
            )
        syserr("pipe\n");

    switch (fork())
    {
        case -1:
            syserr("fork\n");

        case 0:
            if ((dup2(CHILD_READ_FD, STDIN_FILENO) == -1) ||
                (dup2(CHILD_WRITE_FD, STDOUT_FILENO) == -1)
                    )
                syserr("dup2\n");

            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    if (close(pipes[i][j]) == -1)
                        syserr("close\n");
                }
            }

            execl("./W", "W", NULL);
            syserr("exec\n");

        default:
            if ((close(CHILD_READ_FD) == -1) ||
                (close(CHILD_WRITE_FD) == -1)
                    )
                syserr("close\n");

            write_to_child_with_check(PARENT_WRITE_FD, input);
            write_to_child_with_check(PARENT_WRITE_FD, stack);
            write_to_child_with_check(PARENT_WRITE_FD, result);

            int buf_len = 0;
            if ((buf_len = read(PARENT_READ_FD, buffer, BUF_SIZE - 2)) == -1)
                syserr("read\n");
            printf("%s", buffer);

            if (wait(0) == -1)
                syserr("wait\n");

            return;
    }
}
