#ifndef AST_H
#define AST_H

#include <stdlib.h>

#include "list.h"

typedef struct ast {
	const char* op;
	//op is any of the operators 
	struct ast* right;
	struct ast* left;
	int len;
	char** val;
	
} ast;

ast* make_ast_value(list* vv);
ast* make_ast_op(const char* op, ast* right, ast* left);
void free_ast(ast* ast);
char** ast_eval(ast* ast);
char* ast_string(ast* ast); 
void print_ast(ast* ast); 

#endif
