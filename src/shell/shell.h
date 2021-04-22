#ifndef SHELL_H
#define SHELL_H

void ln(char* pathReference, char* filename, char currIdx);
void runShell();
void cwd(char parentIndex, char* buffer);
void ls(char currentIndex);
void cat(char* pathReference, char currIdx);
int checkCommand(char* input, char* availableCommand);
void cd(char* path);

#endif