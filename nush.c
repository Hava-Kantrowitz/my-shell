#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "list.h"
#include "tokens.h"

int
printToFile(char* args[]) {
		int new_pid;
		int j = 0; 
		char* commandArgs[256];
		while (*args[j] != '>') {
			commandArgs[j] = args[j];
			j++;
		}

		commandArgs[j] = NULL;

		if ((new_pid = fork())) {
			int status;
			waitpid(new_pid, &status, 0);
		}

		else {
			char* file = args[j+2];
			int fd = open(file, O_CREAT | O_TRUNC | O_WRONLY, 0644);
			close(1);
			dup(fd);
			close(fd); 
			execvp(commandArgs[0], commandArgs); 
		}
	return 0; 

}

int
pipeIn(char* args[]) {
	int rv;
	int pipe_fds[2];

	int p_read = pipe_fds[0];
	int p_write = pipe_fds[1];
	int cpid; 

	if ((cpid = fork())) {
		//this is parent
		close(p_read);
		char msg[] = "Hello, pipe. \n";
		write(p_write, msg, strlen(msg));
	}
	else {
		close(p_write);
		char temp[100];
		rv = read(p_read, temp, 100);
		temp[rv] = 0;

		write(1, temp, strlen(temp));
	}

	return 0; 
}

void
execute(char* cmd)
{
    int cpid; 

    if ((cpid = fork())) {

        int status;
        waitpid(cpid, &status, 0);

        if (WIFEXITED(status)) {
            //printf("child exited with exit code (or main returned) %d\n", WEXITSTATUS(status));
        }
    }
    else {

	//tokenize
	
	list* tokens = tokenize(cmd);
	char* args[256]; 
	int i = 0;
	int hasSemi = 0; 
	int hasRedirOut = 0;
	int hasRedirIn = 0;
	int hasPipe = 0;
	int hasBackground = 0; 
	for (; tokens; tokens = tokens->tail) {
		args[i] = tokens->head;
		if (*args[i] == ';') {
			hasSemi = 1;
		}
		if (*args[i] == '>') {
			hasRedirOut = 1;
		}

		if (*args[i] == '|') {
			hasPipe = 1;
		}
		i++;
	}

	args[i] = NULL;
	if (hasPipe) {
		pipeIn(args); 
	}

	else if (hasBackground) {
		//background code
	}

	else if (hasRedirOut) {
		printToFile(args); 
	}

	/*if (hasPipe) {
		//pipeIn(args);
	}*/
	
	else {
		execvp(args[0], args);
	}


	//process

        // The argv array for the child.
        // Terminated by a null pointer.

        //printf("== executed program's output: ==\n");

	/*else {
		printf("why am I suddenly getting here\n"); 
	execvp(args[0], args);
	}*/

	

    }
}

int
main(int argc, char* argv[])
{
    char cmd[256];

    if (argc == 1) {
        printf("nush$ ");
        fflush(stdout);
        fgets(cmd, 256, stdin);
    }
    else {
        memcpy(cmd, "echo", 5);
    }

    execute(cmd);

    while(1) {
	    printf("nush$ ");
	    char* input = fgets(cmd, 256, stdin);
	    if (!input) {
		    printf("\n");
		    break;
	    }

	    execute(cmd);
    }

    return 0;
}
