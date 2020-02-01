#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "../lib/commandlinereader.h"

#define MAXCHAR 250
#define TAMENSG 1000

int main(int argc, char const *argv[]) {
	char shell_pipe_send[MAXCHAR];
	char *args[4 + 1];
	int numArgs;

	if (argv[1] == NULL) {
		printf("Missing input\n");
		exit(-1);
	}
	strcpy(shell_pipe_send,argv[1]);
	char pipeName[MAXCHAR] = "/tmp/pipeXXXXXX";
	int shell, client, fd;
	char input[MAXCHAR], buffer[TAMENSG];

	/*Creating the pipe name*/
	if ((fd = mkstemp(pipeName)) < 0) {
		printf("Error creating pipe name");
		exit(-1);
	}

	/*Creating the pipe*/
	unlink(pipeName);
	if (mkfifo(pipeName, 0777) != 0) {
		printf("Error creating pipe.\n");
		exit(-1);
	}

	if ((shell = open(shell_pipe_send, O_WRONLY)) < 0) {
		printf("Error opening the Shell pipe.\n");
		exit(-1);
	}

	/*infinite loop*/
	while (1) {
		numArgs = readLineArguments(args, 4, input, MAXCHAR);

		strcpy(buffer,"");

		if (numArgs == 1) {
			strcat(buffer, args[0]);
			strcat(buffer, " ");
			strcat(buffer, args[0]);
			strcat(buffer, " ");
			strcat(buffer, pipeName);
		} else {
			strcat(buffer, args[0]);
			strcat(buffer, " ");
			strcat(buffer, args[1]);
			strcat(buffer, " ");
			strcat(buffer, pipeName);
		}

		int byteWriten = write(shell, buffer, strlen(buffer) + 1);
		if (byteWriten < 0) {
			printf("Error writing to AdvShell.\n");
			exit(-1);
		}

		if ((client = open(pipeName, O_RDONLY)) < 0) {
			printf("Error opening the pipe.\n");
			exit(-1);
		}

		if (read(client, buffer, TAMENSG) < 0) {
			printf("Error reading from AdvShell.\n");
			exit(-1);
		} else {
			printf("%s\n", buffer);
		}
	}

	if (close(client) < 0) {
		printf("Error closing Client pipe\n");
		exit(-1);
	}
	if (close(shell) < 0) {
		printf("Error closing Shell pipe\n");
		exit(-1);
	}
	unlink(pipeName);

	return 0;
}
