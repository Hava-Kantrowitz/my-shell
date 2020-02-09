#ifndef TOKENS_H
#define TOKENS_H

int still_word(const char* line, int index, int inner);
char* read_command(const char* line, int index);
char* read_punct(const char* line, int index);
list* tokenize(const char* line); 

#endif 
