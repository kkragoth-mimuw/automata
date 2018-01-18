#include <stdio.h>
#include <stdlib.h>

#define PARENT_READ_PIPE    pipes[0]
#define PARENT_WRITE_PIPE   pipes[1]
#define PARENT_READ_FD      pipes[0][0]
#define PARENT_WRITE_FD     pipes[1][1]
#define CHILD_READ_FD       pipes[1][0]
#define CHILD_WRITE_FD      pipes[0][1]

void spawn_run();
void pass_automata_desc_to_child(int fd);