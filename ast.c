#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#include "ast.h"
#include "list.h"
#include "nush.h"

ast*
make_ast_value(list* vv) { 
	ast* my_ast = calloc(1, sizeof(ast));
	my_ast->op = "NO";
	my_ast->right = 0;
	my_ast->left = 0;
	my_ast->val = calloc(length(vv), sizeof(char*));
	my_ast->len = length(vv);
	int i = 0;
	for (; vv; vv = vv->tail) {
		my_ast->val[i] = calloc(strlen(vv->head), sizeof(char));
		my_ast->val[i] = strdup(vv->head);
		i++;
	}
	return my_ast;
}

ast*
make_ast_op(const char* op, ast* right, ast* left) {
	ast* my_ast = calloc(1, sizeof(ast));
	my_ast->op = strdup(op);
	my_ast->right = right;
	my_ast->left = left;
	my_ast->len = 9000;
	return my_ast;
}

void
free_ast(ast* ast) {
	if (ast) {
		if (ast->right) {
			free(ast->right);
		}

		if (ast->left) {
			free(ast->left);
		}

		if(ast->val) {
			for (int i = 0; i < ast->len; i++) {
			free(ast->val[i]);
		}

		free(ast->val); 
		}

		free(ast);

	}
}

char**  
ast_eval(ast* ast) {
	if (strcmp(ast->op, "NO") == 0) {
		
		return ast->val;

	}

	else if (*ast->op == '|') {
		int cpid;
		if ((cpid = fork())) {
			int status;
			waitpid(cpid, &status, 0);
		}
		else {
			pipeIn(ast_eval(ast->right), ast_eval(ast->left));
		}
		return(ast->val);

	}

	else if (*ast->op == '&') {
		//here
	}

	else if (*ast->op == '<') {
		int cpid;
		if ((cpid = fork())) {
			int status;
			waitpid(cpid, &status, 0);
		}
		else {
			getFromFile(ast_eval(ast->right), ast_eval(ast->left));
		}
		return(ast->val);

	}

	else if (*ast->op == '>') {
		int cpid;
		if ((cpid = fork())) {
			int status;
			waitpid(cpid, &status, 0);
		}
		else {
			printToFile(ast_eval(ast->right), ast_eval(ast->left));
		}
		return(ast->val);

	}

	else if (*ast->op == ';') {
	        int cpid;
		if ((cpid = fork())) {
			int status;
			waitpid(cpid, &status, 0);
		}
		else {
			runSemi(ast_eval(ast->right), ast_eval(ast->left));
		}
	        return(ast->val); 	
	}

	else if (strcmp(ast->op, "&&") == 0) {
		int cpid;
		if ((cpid = fork())) {
			int status;
			waitpid(cpid, &status, 0);
		}
		else {
			runAnd(ast_eval(ast->right), ast_eval(ast->left));
		}
		return(ast->val);
	}

	else if (strcmp(ast->op, "||") == 0) {
		int cpid;
		if ((cpid = fork())) {
			int status;
			waitpid(cpid, &status, 0);
		}
		else {
			runOr(ast_eval(ast->right), ast_eval(ast->left));
		}
		return(ast->val);

	}

	return 0;
}

char*
ast_string(ast* ast) {
	if (strcmp(ast->op, "NO") == 0) {
		char* tmp = calloc(ast->len, sizeof(char));
		// Loop thru ast->val (char**)
		// strcat(val[i] onto tmp)
		for (int i = 0; i  < ast->len; i++) {
			strcat(tmp, ast->val[i]); 
			strcat(tmp, " ");
		}
		return tmp;
	}
	else {
		char* aa = ast_string(ast->right);
		char* bb = ast_string(ast->left);
		char* cc = malloc(128);
		sprintf(cc, "(%s %s %s)", aa, ast->op, bb);
		free(aa);
		free(bb);
		return cc;
	}
}

void 
print_ast(ast* ast) {
	char* text = ast_string(ast);
	printf("%s\n", text);
	free(text);
}
