//NOTE: ATTRIBUTION: The main function is based on the main function written
//in class by Nat Tuck, CS3650 Spring 2020
//The read_command function is based on the read_int function written in class
//by Nat Tuck, CS3650 Spring 2020
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "tokens.h"

int
still_word(const char* line, int index, int inner) {
	int isWord = 1;
	if (isspace(line[index + inner])) {
		isWord = 0;
	}

	else if (ispunct(line[index + inner])) {
		if(line[index + inner] == '.' || line[index + inner] == '/'
				|| line[index + inner] == '_' 
				|| line[index + inner] == '-') {
			return isWord;
		}
		isWord = 0;
	}

	return isWord;
}

char*
read_command(const char* line, int index) {
	int inner = 0;
	while(still_word(line, index, inner)){
		inner++;
	}

	char* command = malloc(inner + 1);
	memcpy(command, line + index, inner);
	command[inner] = 0;
	return command;
}

char*
read_punct(const char* line, int index) {
	int inner = 0;
	while(ispunct(line[index + inner])){
		inner++;
	}

	char* command = malloc(inner + 1);
	memcpy(command, line + index, inner);
	command[inner] = 0;
	return command;
}

list* 
tokenize(const char* line) {
	list* alist = 0;

	int len = strlen(line);
	int index = 0;

	while (index < len) {
		if (isspace(line[index])) {
			index++;
			continue;
		}

		if(ispunct(line[index]) && line[index] != '/' 
				&& ispunct(line[index]) && line[index] != '-') {
			char* punc_token = read_punct(line, index);
			alist = cons(punc_token, alist);
			index += strlen(punc_token);
			free(punc_token);
		}


			char* token = read_command(line, index);
			alist = cons(token, alist);
			index += strlen(token);
			free(token);
		
	}

	list* token_list = rev_free(alist);

	return token_list;
}
