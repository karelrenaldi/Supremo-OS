#ifndef __FILE__
#define __FILE__

void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void readFile(char *buffer, char *path, int *result, char parentIndex);

#endif