/* This file is lecture notes from CS 3650, Fall 2018 */
/* Author: Nat Tuck */

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "list.h"

list*
cons(char* xx, list* old_list) {
	list* new_list = malloc(sizeof(list));
	new_list->head = strdup(xx);
	new_list->tail = old_list;
	return new_list;
}

void
free_list(list* alist)
{
    if (alist) {
	    free(alist->head);
	    free(alist->tail);
	    free(alist);
    }
}

void
print_list(list* alist) {
	for (; alist; alist = alist->tail) {
		printf("%s\n", alist->head);
	}
}

int
length(list* alist) {
	int length = 0;
	for (; alist; alist = alist->tail) {
		length++;
	}
}

list*
reverse(list* alist) {
	list* rev_list = 0;
	for (; alist; alist = alist->tail) {
		rev_list = cons(alist->head, rev_list);
	}
	return rev_list;
}

list* 
rev_free(list* alist) {
	list* rev_list = reverse(alist);
	free(alist);
	return rev_list;
}


