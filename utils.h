#ifndef UTILS_H
#define UTILS_H

int lengthString(char *str);
int isSameString(char *str1, char *str2);
char idxPath(char *path, char *files, char parentIndex);
char getCurrentIndex(char *name, char *files, char parentIndex);
void ln(char* pathReference, char* pathName);
void printString(char *string);

#endif