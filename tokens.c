
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

list* 
tokenize(const char* line) {
	list* alist = 0;

	int len = strlen(line);
	int index = 0;

	char* token = calloc(50, sizeof(char));
	while (index < len) {
		while(!isspace(line[index])) {
			if (strcmp(&line[index], ";") == 0) {
				break;
			}
			token = strncat(token, &line[index], 1); 
			index++;
		}


		alist = cons(token, alist);
		index++;
	}

	free(token);
	return rev_free(alist);
}


int 
main(int _ac, char** _av) {
	char line[100];
	while (1) {

	  printf("tokens$ ");
	  fflush(stdout);
	  char* input = fgets(line, 96, stdin);
     	  if (!input) {
		  printf("\n");
		  break;
	  }

	  //printf("%s", input);
     	
	  list* tokens  = tokenize(line);

	  print_list(tokens); 

	}
}
