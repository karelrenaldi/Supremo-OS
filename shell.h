#ifndef SHELL_H
#define SHELL_H

void runShell();
void cwd(char parentIndex, char* buffer);
void ls(char currentIndex);
void cat(char* pathReference, char currIdx);

#endif