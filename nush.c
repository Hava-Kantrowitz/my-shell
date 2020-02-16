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
getFromFile(char* args[]) {
	int new_pid; 
	int j = 0;
	char* commandArgs[256];
	while (*args[j] != '<') {
		commandArgs[j] = args[j];
		j++;
	}

	commandArgs[j] = NULL;
	char* file = args[j+2];

	if ((new_pid = fork())) {
		int status;
		waitpid(new_pid, &status, 0);
	}

	else {
		//FILE *input = fopen(file, "r"); 
		//fscanf(input, "%[^\n]", cmd);
		//while (fgets(cmd, sizeof(cmd), input)) {
			 
		//}

		//fclose(input); 

		char* cmd[256];
	        	
		int fd = open(file, O_RDONLY);
		close(0);
		dup(fd);
		close(fd);
		printf("hi\n"); 
		//char *  in  = fgets(cmd, sizeof(cmd), 0);
		//execvp(commandArgs[0], in); 
		int len = read(0, cmd, sizeof(cmd));
		cmd[len] = NULL;
		printf("cmd[0] is %s\n", cmd[0]);
		printf("cmd[1] is %s\n", cmd[1]);
		printf("cmd[2] is %s\n", cmd[2]); 
	       // execvp(commandArgs[0], cmd); 
	}
	return 0;
} 

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
runCD(char* args[]) {
	chdir(args[1]); 
       	return 0; 	
}

int
runOr(char* args[]) {
	//run first one, run second only if A failed
	//get both args groups
	int j = 0;
	char* firstArgs[256];
	while (strcmp(args[j], "||")) {
		firstArgs[j] = args[j];
		j++;
	}

	firstArgs[j] = NULL;

	j+= 2;
	char* secondArgs[256];
	int i = 0; 
	while (args[j] != NULL) {
		secondArgs[i] = args[j];
		j++;
		i++;
	}

	secondArgs[i] = NULL;

	int cpid;
	int succeeded = 1; 
	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);
		if (strcmp(firstArgs[0], "false") == 0) {
			succeeded = 0;
		}

		if (!succeeded) {
			execvp(secondArgs[0], secondArgs); 
		}

	}
	else {
		execvp(firstArgs[0], firstArgs); 
		succeeded = 0; 
	}
	return 0;
}


int
runAnd(char* args[]) {
	//run first one, run second only if A succeeded
	//get both args groups
	int j = 0;
	char* firstArgs[256];
	while (strcmp(args[j], "&&")) {
		firstArgs[j] = args[j];
		j++;
	}

	firstArgs[j] = NULL;

	j+= 2;
	char* secondArgs[256];
	int i = 0; 
	while (args[j] != NULL) {
		secondArgs[i] = args[j];
		j++;
		i++;
	}

	secondArgs[i] = NULL;
	int cpid;
	int succeeded = 1;
	if ((cpid = fork())) {
		if (strcmp(firstArgs[0], "false") == 0) {
			succeeded = 0;
		}
		int status;
		waitpid(cpid, &status, 0);

		if (succeeded) {
			execvp(secondArgs[0], secondArgs);
		}	
	}
	else {
		execvp(firstArgs[0], firstArgs);
		succeeded = 0; 
	}
	return 0;
}

int
runSemi(char* args[]) {
	//get the args
	int j = 0;
	char* firstArgs[256];
	while (*args[j] != ';') {
		firstArgs[j] = args[j];
		j++;
	}

	firstArgs[j] = NULL;

	j+= 2;
	char* secondArgs[256];
	int i = 0;
	while (args[j] != NULL) {
		secondArgs[i] = args[j];
		j++;
		i++;	
	}
	secondArgs[i] = NULL;

	int cpid;
	if ((cpid = fork())) {
		execvp(firstArgs[0], firstArgs);
	}
	else {
		execvp(secondArgs[0], secondArgs); 
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

	//get first args
	int j = 0; 
	char* firstArgs[256];
	while (*args[j] != '|') {
		firstArgs[j] = args[j];
		j++;
	}
	firstArgs[j] = NULL;

	//get second args
	j+= 2;
	char* secondArgs[256];
	int i = 0;
	while (args[j] != NULL) {
		secondArgs[i] = args[j];
		j++;
		i++;	
	}
	secondArgs[i] = NULL;

	if ((cpid = fork())) {
		//this is parent
		close(p_write);

		int status;
		waitpid(cpid, &status, 0);
	}
	else {
		//child
		close(p_read);
		close(1);
		dup(p_write);
		execvp(secondArgs[0], secondArgs); 
	}

	return 0; 
}

int
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
        int hasAnd = 0;
	int hasOr = 0;
	int hasCD = 0;	
	int hasExit = 0; 
	for (; tokens; tokens = tokens->tail) {
		args[i] = tokens->head;
		if (*args[i] == ';') {
			hasSemi = 1;
		}
		if (*args[i] == '>') {
			hasRedirOut = 1;
		}

		if (*args[i] == '<') {
			hasRedirIn = 1;
		}

		if (*args[i] == '|') {
			if (strcmp(args[i], "||") == 0) {
				hasOr = 1;
			}
			else {
			hasPipe = 1;
			}
		}

		if (*args[i] == '&') {
			if (strcmp(args[i], "&&") == 0) {
				hasAnd = 1;
			}

			else {
				hasBackground = 1; 
			}
		}

		if (strcmp(args[i], "cd") == 0) {
			hasCD = 1;
		}

		if (strcmp(args[i], "exit") == 0) {
			hasExit = 1;
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

	else if (hasSemi) {
		runSemi(args);
	}

	else if (hasAnd) {
		runAnd(args);
	}

	else if (hasOr) {
		runOr(args);
	}

	else if (hasRedirIn) {
		getFromFile(args);
	}

	else if (hasCD) {
		runCD(args); 
	}

	else if (hasExit) {
		//exit code	
	}

	
	else {
		execvp(args[0], args);
	}


    }

    return 0;
}

int
main(int argc, char* argv[])
{
    char cmd[256];
    int ex = 0;

    if (argc == 1) {
        printf("nush$ ");
        fflush(stdout);
        fgets(cmd, 256, stdin);
	ex = execute(cmd); 
	if (ex) {
		printf("RIGHT HERE RIGHT HERE\n"); 
		ex = 1; 
		exit(0); 
		printf("hi here\n"); 
	}
    }
    else {
	char* file = argv[1];
	//int openFile = open(file, O_RDONLY);
	
	FILE *input = fopen(file, "r"); 
	//fscanf(input, "%[^\n]", cmd);
	while (fgets(cmd, sizeof(cmd), input)) {
	ex = execute(cmd); 
	if (ex) {
		ex = 1;
	}
	}

	fclose(input); 
    }

    printf("we print here\n"); 

    //ex = 1; 
    if (ex) {
	    printf("we are here\n"); 
	    exit(0); 
    }


    while(1) {
	    printf("nush$ ");
	    char* input = fgets(cmd, 256, stdin);
	    if (!input) {
		    printf("\n");
		    break;
	    }

	    ex = execute(cmd);
	    if (ex) {
		    break;    
	    }
    }

    return 0;
}
