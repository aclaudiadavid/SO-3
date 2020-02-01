#ifndef CIRCUITROUTER_SHELL_H
#define CIRCUITROUTER_SHELL_H

#include "../lib/vector.h"
#include <sys/types.h>
#include <time.h>

typedef struct {
    pid_t pid;
    int status;
	time_t begins;
	time_t ends;
} child_t;

void childEnded(int);
void printChildren();

#endif /* CIRCUITROUTER_SHELL_H */
