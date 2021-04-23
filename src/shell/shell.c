#include "shell.h"
#include "../utils/utils.h"
#include "../definition/definition.h"

char currentShellIdx = 0xFF;

void cwd(char currIdx, char* currentDirectory);
void cd(char *path, char currentShellIdx);
int checkCommand(char* input, char* availableCommand);
void getCommand(char* input, char* command);
void messagePassing(char* argv, char parentIndex);

int main() {
  int i, argsCurrent, argsNeeded, flag, idx;
  char status[16], args1[128], args2[128], command[128], buffer[512*16], argsIdx[128];
  char files[1024];
  char *currentDirectory = "";
  char *input = "";

  interrupt(0x21, 2, files, 0x101, 0);
  interrupt(0x21, 2, files + 512, 0x102, 0);
  idx = idxPath("rpl/~msg", files, 0xFF);
  if(idx != 0x40){
    readFile(buffer, "rpl/~msg", &flag, 0xFF);
    getSplittedStringOnIndex(argsIdx, buffer, '-', 1);
    currentShellIdx = argsIdx[0];
    deleteFile(idx);
  }

  
  while(1) {
    clear(command, 128);
    clear(args1, 128);

    cwd(currentShellIdx, currentDirectory);
    interrupt(0x21, 1, input, 0, 0);

    getCommand(input, command);

    if(checkCommand(input, "cd")) {
      argsCurrent = 0, argsNeeded = 1, i = 0;

      while(*(input + i) != '\0') {
        if(*(input + i) == ' ' && *(input + i + 1) != ' ' && *(input + i + 1) != '\0') {
          argsCurrent++;
        }
        i++;
      }

      if(argsCurrent == argsNeeded) {
        getSplittedStringOnIndex(args1, input, ' ', 1);
        cd(args1);
      }else if (argsCurrent > argsNeeded){
        printString("to many arguments");
      }else{
        printString("to few arguments");
      }
    }else{
      messagePassing(input, currentShellIdx);
      interrupt(0x21, 0x06, command, 0x3000, status);
    }
  }

  return 0;
}

void messagePassing(char* argv, char parentIndex) {
  char buffer[512], files[1024];
  int i, j, argc, flag, idx;

  clear(buffer, 512);
  i = 0;
  j = 0;
  argc = 1;
  while(argv[i] != '\0') {
    if(argv[i] == ' '){
      if(j == 0) {
        j = i + 1;
      }
      argc++;
    }
    i++;
  }

  buffer[0] = argc;
  buffer[1] = '-';
  buffer[2] = parentIndex;
  buffer[3] = '-';

  i = 0;
  while(argv[j + i] != '\0'){
    if(argv[j + i] == ' '){
      buffer[4 + i] = '-';
    }else{
      buffer[4 + i] = argv[j + i];
    }
    i++;
  }

  buffer[4 + i] = '\0';

  interrupt(0x21, 2, files, 0x101, 0);
  interrupt(0x21, 2, files + 512, 0x102, 0);

  idx = idxPath("rpl", files, 0xFF);
  writeFile(buffer, "~msg", &flag, idx);
}

void cwd(char currIdx, char* currentDirectory) {
  int i, idx = 0, neff = 0, j;
  char pathIndexArray[128], files[1024];
  char currentIndex = currIdx;
  char currentParentIndex;

  if(currIdx == 0xFF) {
    currentDirectory[idx++] = '/';
  }else{
    currentDirectory[idx++] = '/';

    interrupt(0x21, 2, files, 0x101, 0);
    interrupt(0x21, 2, files + 512, 0x102, 0);

    while(currentIndex != 0xFF) {
      pathIndexArray[neff] = currentIndex;
      currentParentIndex = files[currentIndex * 16];
      currentIndex =  currentParentIndex;
      neff++;
    }

    i = neff - 1;
    while(i >= 0){
      j = 0;
      while(files[(pathIndexArray[i] * 16) + 2 + j] != '\0') {
        currentDirectory[idx++] = files[(pathIndexArray[i] * 16) + 2 + j];
        j++;
      }
      currentDirectory[idx++] = '/';
      i--;
    }
  }

  currentDirectory[idx] = '$';
  currentDirectory[idx + 1] = ' ';
  currentDirectory[idx + 2] = 0;

  i = 0;
  while (currentDirectory[i] != 0)
  {
    interrupt(0x10, (0x0e << 8) + currentDirectory[i], 0x0, 0x0, 0x0);
    i++;
  }
}

void cd(char *path)
{
  char files[1024];
  char pathIdx = 0xFF;

  interrupt(0x21, 2, files, 0x101, 0);
  interrupt(0x21, 2, files + 512, 0x102, 0);

  pathIdx = idxPath(path, files, currentShellIdx);

  if (pathIdx == 0x40){ // Not found index
    interrupt(0x21, 0x0, "Path not found.", 0x0, 0x0);
  } else {
    if(pathIdx == 0xFF) {
      currentShellIdx = pathIdx;
    }else if(files[(pathIdx * 16) + 1] == 0xFF) {
      currentShellIdx = pathIdx;
    }else{
      interrupt(0x21, 0x0, "Path is not a directory", 0x0, 0x0);
    }
  }
}

int checkCommand(char* input, char* availableCommand) {
  char command[128];
  int i = 0;

  while(input[i] != '\0' && input[i] != ' ') {
    *(command + i) = input[i];
    i++;
  }
  *(command + i) = '\0';

  if(isSameString(command, availableCommand)) {
    return 1;
  }else{
    return 0;
  }
}

void getCommand(char* input, char* command) {
  int i = 0;

  while(input[i] != '\0' && input[i] != ' ') {
    *(command + i) = input[i];
    i++;
  }

  *(command + i) = '\0';
}