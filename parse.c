#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "ast.h"
#include "parse.h"
#include "list.h"

int
streq(const char* aa, const char* bb) {
	return strcmp(aa, bb) == 0;
}

int
find_last_index(list* toks, const char* tt) {
	int ii = 0;
	int last_index = -1;
	for (list* it = toks; it; it = it->tail) {
		if (streq(it->head, tt)) {
			last_index = ii;
		}
		ii++;
	}

	return last_index;
}

int
contains(list* toks, const char* tt) {
	return find_last_index(toks, tt) >= 0;
}

list*
slice(list* xs, int i0, int i1) {
	list* ys = 0;
	list* it = xs;
	for (int ii = 0; ii < i0; ii++) {
		it = it->tail;
	}
	for (int ii = i0; ii < i1; ii++) {
		ys = cons(it->head, ys);
		it = it->tail;
	}
	return rev_free(ys);
}

ast*
parse(list* toks) {

	const char* ops[] = {"|", "||", "&", "&&", "<", ">", ";"};
	
	for (int ii = 0; ii < 7; ii++) {
		const char* op = ops[ii];

		if (contains(toks, op)) {
			int jj = find_last_index(toks, op);
			int len = length(toks); 
			list* xs = slice(toks, 0, jj);
			list* ys = slice(toks, jj+1, len);
			ast* ast = make_ast_op(op, parse(xs), parse(ys));
			free_list(xs);
			free_list(ys);
			return ast;
		}
	}

	return make_ast_value(toks); 

	fprintf(stderr, "Invalid token stream\n");
	exit(1);
}
