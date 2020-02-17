/*NOTE: The shell code is built off of the shell code written by Nat Tuck in class two Thursday's ago, 
 * last Tuesday, and last Thursday. I'm only writing it here since this is the main program
 * but THIS HOLDS FOR EVERY FILE IN THIS PROGRAM. I'm also writing this in the notes section when I
 * submit. Thank you!
 * CREDIT: NAT TUCK, CLASS NOTES
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "list.h"
#include "tokens.h"
#include "ast.h"
#include "parse.h" 

int
getFromFile(char** firstArgs, char** secondArgs) {
	int new_pid; 
	
	if ((new_pid = fork())) {
		int status;
		waitpid(new_pid, &status, 0);
	}

	else {
		char* file = firstArgs[0];
		int fd = open(file, O_RDONLY);
		close(0);
		dup(fd);
		close(fd);
		execvp(secondArgs[0], secondArgs); 
	}
	return 0;
} 

int
printToFile(char** firstArgs, char** secondArgs) {
		int new_pid;

		if ((new_pid = fork())) {
			int status;
			waitpid(new_pid, &status, 0);
		}

		else {
			char* file = firstArgs[0];
			int fd = open(file, O_CREAT | O_TRUNC | O_WRONLY, 0644);
			close(1);
			dup(fd);
			close(fd); 
			execvp(secondArgs[0], secondArgs); 
		}
	return 0; 

}

int
runOr(char** firstArgs, char** secondArgs) {
	//run first one, run second only if A failed

	int cpid;
	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);

		if (status != 0) {
			execvp(firstArgs[0], firstArgs); 
		}

	}
	else {
		execvp(secondArgs[0], secondArgs); 
	}
	return 0;
}


int
runAnd(char** firstArgs, char** secondArgs) {
	//run first one, run second only if A succeeded
	//SecondArgs is left arg tree 
	int cpid;
	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);
		
		if (status == 0) {
			execvp(firstArgs[0], firstArgs);
		}	
	}
	else {
		execvp(secondArgs[0], secondArgs);
	}
	return 0;
}

int
runSemi(char** firstArgs, char** secondArgs) {

	int cpid;
	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0); 

		execvp(secondArgs[0], secondArgs);
	}
	else {
		execvp(firstArgs[0], firstArgs); 
	}
	return 0;
}

int
runBackground(char** firstArgs, char** secondArgs) {
	int cpid;
	if ((cpid = fork())) {
		int hi;
		waitpid(cpid, &hi, 0); 
		execvp(secondArgs[0], secondArgs);
	}
	else {
		execvp(firstArgs[0], firstArgs);
	}

	return 0; 
}

int
pipeIn(char** firstArgs, char** secondArgs) {
	int cpid; 
	int pipes[2]; 

	pipe(pipes); 
	int stout = dup(1);
	int stin = dup(0); 
	close(1);
	dup(pipes[1]); 
	close(pipes[1]);

	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);
	}
	else {
		execvp(secondArgs[0], secondArgs);
	}

	close(1);
	dup(stout);
	close(stout); 
	
	close(0);
	dup(pipes[0]);
	close(pipes[0]);

	if ((cpid = fork())) {
		int hi;
		waitpid(cpid, &hi, 0);
	}
	else {
		char* args[] = {firstArgs[0], firstArgs[1], firstArgs[2], 0};
		execvp(firstArgs[0], args);
	}

	close(0);
	dup(stin);
	close(stin);
	return 0; 
}

void
execute(ast* ast) {
	int cpid;
	int new_pid;

	if (strcmp(ast->op, "NO") == 0) {
		if (strcmp(*ast->val, "exit") == 0) {
			exit(0);
		}

		if (strcmp(*ast->val, "cd") == 0) {
			chdir(ast->val[1]);
		}
	}

	if (*ast->op == '&') {
		int newpid;
		if((newpid = fork())) {
			char** first = ast_eval(ast->left);
			execvp(first[0], first);
		}
		else {
			char** second = ast_eval(ast->right);
			execvp(second[0], second);
		}
	}


	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);

	}
	else {
		if (strcmp(ast->op, "NO") == 0) {
			ast->val[ast->len] = NULL; 
			execvp(ast->val[0], ast->val);
		}

		else if (*ast->op == ';') {
			runSemi(ast_eval(ast->right), ast_eval(ast->left)); 
		}

		else if (*ast->op == '>') {
			printToFile(ast_eval(ast->left), ast_eval(ast->right));
		}

		else if (*ast->op == '<') {
			getFromFile(ast_eval(ast->left), ast_eval(ast->right));
		}

		else if (*ast->op == '&') {
			if (strcmp(ast->op, "&&") == 0) {
				runAnd(ast_eval(ast->left), ast_eval(ast->right));
			}

			else {
				runBackground(ast_eval(ast->left), ast_eval(ast->right));
			}
		}

		else if (*ast->op == '|') {
			if (strcmp(ast->op, "||") == 0) {
				runOr(ast_eval(ast->left), ast_eval(ast->right));
			}

			else {
				pipeIn(ast_eval(ast->left), ast_eval(ast->right));
			}
		}


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
	list* toks = tokenize(cmd);
	ast* my_ast = parse(toks);
//	print_ast(my_ast); 
	execute(my_ast); 
    }
    else {
	char* file = argv[1];
	
	FILE *input = fopen(file, "r"); 
	while (fgets(cmd, sizeof(cmd), input)) {
	list* toks = tokenize(cmd);
	ast* my_ast = parse(toks);
	execute(my_ast); 
	}

	fclose(input); 
    }


    while(1) {
	    printf("nush$ ");
	    char* input = fgets(cmd, 256, stdin);
	    if (!input) {
		    printf("\n");
		    break;
	    }

	    list* toks = tokenize(cmd);
	    ast* my_ast = parse(toks);
	    execute(my_ast);
    }

    return 0;
}
