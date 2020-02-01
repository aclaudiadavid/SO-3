/*
// Projeto SO - exercise 1, version 1
// Sistemas Operativos, DEI/IST/ULisboa 2018-19
*/
#include "../lib/commandlinereader.h"
#include "../lib/vector.h"
#include "CircuitRouter-AdvShell.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <time.h>

#define COMMAND_EXIT "exit"
#define COMMAND_RUN "run"
#define PIPEPATH "/tmp/CircuitRouter-AdvShell.pipe"

#define MAXARGS 4
#define BUFFER_SIZE 100

vector_t *children;
int runningChildren = 0;
int MAXCHILDREN = -1;

/*To do:
remake waitForChild*/
void waitForChild() {
	pid_t child_pid;
	int childStatus;

    while (1) {
		child_t *child = malloc(sizeof(child_t));

        child_pid = wait(&childStatus);

        if (child_pid < 0) {
            if (errno == EINTR) {
                /* Este codigo de erro significa que chegou signal que interrompeu a espera
                   pela terminacao de filho; logo voltamos a esperar */
				free(child);
                continue;
            } else {
                perror("Unexpected error while waiting for child.");
                exit (EXIT_FAILURE);
            }
        }

        return;
    }
}

void storeChild(pid_t pid, time_t start) {
	child_t *child = malloc(sizeof(child_t));

	if (child == NULL) {
		perror("Error allocating memory.");
		exit(EXIT_FAILURE);
	}

	child->pid = pid;
	child->begins = start;
	vector_pushBack(children, child);
}

void childInterrupt(int sig) {
	signal(SIGCHLD, childInterrupt);
	runningChildren--;
	time_t end;
	time(&end);
	pid_t pid;
	int status;

	pid = waitpid(-1, &status, WNOHANG);

	if (pid < 0) {

		return;
	} /*falta opcoes*/

	if (WIFEXITED(status)) {
		for (int i = 0; i < vector_getSize(children); ++i) {
			child_t *child = vector_at(children, i);

			if (child->pid == pid) {
				if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
					child->ends = end;
					break;
				} else if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
					child->ends = end;
				}
			}
		}
	}
}

void printChildren() {
    for (int i = 0; i < vector_getSize(children); ++i) {
        child_t *child = vector_at(children, i);
        int status = child->status;
		double diffTime;
        pid_t pid = child->pid;

        if (pid != -1) {
            const char* ret = "NOK";
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                ret = "OK";
            }
			diffTime = difftime(child->ends, child->begins);

			printf("CHILD EXITED: (PID=%d; return %s; %0.f s)\n", pid, ret, diffTime);
        }
    }
    puts("END.");
}

int parseInputs(int numArgs, char *args[], int pipe) {

	if (numArgs < 0 || (numArgs > 0 && (strcmp(args[0], COMMAND_EXIT) == 0))) {

		if (pipe == 0) {
			printf("CircuitRouter-AdvShell will exit.\n--\n");

			/* Espera pela terminacao de cada filho */
			while (runningChildren > 0) {
				waitForChild();
				runningChildren --;
			}

			printChildren();
			printf("--\nCircuitRouter-AdvShell ended.\n");
			exit(0);
		} else {
			int client;

			if (((client = open(args[2], O_WRONLY)) < 0)) {
				printf("Error opening Client pipe.\n");
				exit(-1);
			} else {
				if (write(client, "Invalid Command.", strlen("Invalid Command.") + 1) < 0) {
					printf("Error writing to Client pipe.\n");
					exit(-1);
				}

				if (close(client) < 0) {
					printf("Error closing Client pipe.\n");
					exit(-1);
				}
			}
		}
	}

	else if (numArgs > 0 && strcmp(args[0], COMMAND_RUN) == 0){
		int pid;

		if (numArgs < 2) {
			printf("%s: invalid syntax. Try again.\n", COMMAND_RUN);
			return 0;
		}
		if (MAXCHILDREN != -1 && runningChildren >= MAXCHILDREN) {
			waitForChild();
			runningChildren--;
		}

		pid = fork();
		if (pid < 0) {
			perror("Failed to create new process.");
			exit(EXIT_FAILURE);
		}

		if (pid > 0) {
			time_t start;
			time(&start);
			storeChild(pid, start);
			runningChildren++;
			printf("%s: background child started with PID %d.\n\n", COMMAND_RUN, pid);
			return 0;
		} else {
			char seqsolver[] = "./CircuitRouter-SeqSolver/CircuitRouter-SeqSolver";

			if (pipe == 0) {
				execl(seqsolver, seqsolver, args[1], "NULL", NULL);
				perror("Error while executing child process"); // Nao deveria chegar aqui
				exit(EXIT_FAILURE);
			} else {
				execl(seqsolver, seqsolver, args[1], args[2], NULL);
				perror("Error while executing child process"); // Nao deveria chegar aqui
				exit(EXIT_FAILURE);
			}
		}
	} else {
		if (pipe == 0) {
			printf("Unknown command. Try again.\n");
		} else {
			int client;
			char message[256];

			if (((client = open(args[2], O_WRONLY)) < 0)) {
				printf("Error opening Client pipe.\n");
				exit(-1);
			} else {
				strcpy(message, "Invalid Command.");
				if (write(client, message, strlen(message) + 1) < 0) {
					printf("Error writing to Client pipe.\n");
					exit(-1);
				}

				if (close(client) < 0) {
					printf("Error closing Client pipe.\n");
					exit(-1);
				}
				return 0;
			}
		}
		return -1;
	}
	return -1;
}

int main (int argc, char** argv) {
	char *args[MAXARGS + 1];
    char buffer[BUFFER_SIZE];
	int myPipe, retVal;
	fd_set rfds;

    if(argv[1] != NULL){
        MAXCHILDREN = atoi(argv[1]);
    }

    children = vector_alloc(MAXCHILDREN);

	 printf("Welcome to CircuitRouter-AdvShell\n\n");

	/*Creating the pipe*/
	unlink(PIPEPATH);
	if (mkfifo(PIPEPATH, 0777) != 0) {
		printf("Error creating pipe.\n");
		exit(-1);
	}
	if ((myPipe = open(PIPEPATH, O_RDONLY)) < 0) {
		printf("Error opening pipe.\n");
		exit(-1);
	}

	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	FD_SET(myPipe, &rfds);

    while (1) {
		signal(SIGCHLD, childInterrupt);
        int numArgs;

		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		FD_SET(myPipe, &rfds);
		retVal = select(myPipe+1, &rfds, NULL, NULL, NULL);

		for (int i = 0; i<retVal; i++) {
			if (FD_ISSET(0, &rfds)){

				numArgs = readLineArguments(args, MAXARGS+1, buffer, BUFFER_SIZE);

				parseInputs(numArgs, args, 0);
			}

	        if (FD_ISSET(myPipe, &rfds)) {
				char clientPipe[BUFFER_SIZE], file[BUFFER_SIZE], command[BUFFER_SIZE];
	            if (read(myPipe, buffer, BUFFER_SIZE) < 0) {
					printf("Error reading from pipe.\n");
					exit(-1);
				}

				strcpy(command," ");
				strcpy(file," ");
				strcpy(clientPipe," ");

				if (sscanf(buffer, "%s%s%s", command, file, clientPipe) != 3){
					printf("Missing pipe inputs.\n");
				}

				char *params[3] = {command, file, clientPipe};

				parseInputs(3, params, 1);
	        }
		}
    }

    for (int i = 0; i < vector_getSize(children); i++) {
        free(vector_at(children, i));
    }
    vector_free(children);

	if (close(myPipe) < 0) {
		printf("Error closing pipe\n");
		exit(-1);
	}
	unlink("CircuitRouter-AdvShell.pipe");

    return EXIT_SUCCESS;
}
