#ifndef LIST_H
#define LIST_H

//Note: Attribution: The list structure is created by Nat Tuck in 
//class lecture notes, CS3650, Spring 2020

typedef struct list {
	int size;
	char* head;
        struct list* tail;	
} list;

list* cons(char* op, list* alist);
void free_list(list* list);
void print_list(list* list);
int length(list* alist);
list* reverse(list* alist);
list* rev_free(list* alist); 

#endif
