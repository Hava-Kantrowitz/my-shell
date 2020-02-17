#ifndef NUSH_H
#define NUSH_H

int getFromFile(char** firstArgs, char** secondArgs);
int printToFile(char** firstArgs, char** secondArgs);
int runOr(char** firstArgs, char** secondArgs);
int runAnd(char** firstArgs, char** secondArgs);
int runSemi(char** firstArgs, char** secondArgs);
int pipeIn(char** firstArgs, char** secondArgs);

#endif
