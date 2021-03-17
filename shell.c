#include "shell.h"
#include "utils.h"

char currentShellIdx = 0xFF;

void ln(char* pathReference, char* filename, char currIdx) {
  char files[1024];
  char pathReferenceIdx, filenameIdx;
  int i, emptyIdx = -1;

  interrupt(0x21, 2, files, 0x101, 0);
  interrupt(0x21, 2, files + 512, 0x102, 0);

  pathReferenceIdx = idxPath(pathReference, files, currIdx);
  filenameIdx = idxPath(filename, files, currIdx);

  if(filenameIdx != 0x40) {
    printString("File sudah ada!");
  }else if(idxPath == 0x40){
    printString("File tidak ditemukan");
  }else{
    for(i = 0; i < 64; i++) {
      if(files[(i * 16) + 2] == 0) {
        emptyIdx = i;
        break;
      }
    }

    if(emptyIdx == -1) {
      printString("Entry file sudah penuh");
      return;
    }

    files[emptyIdx * 16] = currIdx;
    files[(emptyIdx * 16) + 1] = files[(pathReferenceIdx * 16) + 1];

    i = 0;
    while (i < 14 && filename[i] != 0){
      files[(emptyIdx * 16) + 2 + i] = filename[i];
      i++;
    }

    interrupt(0x21, 3, files, 0x101, 0);
    interrupt(0x21, 3, files + 512, 0x102, 0);
  }
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

void ls(char currentIndex){
  char files[1024];
  int i, j;

  interrupt(0x21, 2, files, 0x101, 0);
  interrupt(0x21, 2, files + 512, 0x102, 0);

  for (i = 0; i < 64; i++){
    j = 0;
    
    if(files[i*16] == currentIndex){
      if(files[i*16 + 1] == 0xFF){
        while(files[i*16 + 2 + j] != 0){
            interrupt(0x10, (0x0e << 8) + files[i*16 + 2 + j], 0x0, 0x0, 0x0);
            j++;
        } 
        interrupt(0x10, (0x0e << 8) + '/', 0x0, 0x0, 0x0);
      } else {
        while(files[i*16 + 2 + j] != 0){
            interrupt(0x10, (0x0e << 8) + files[i*16 + 2 + j], 0x0, 0x0, 0x0);
            j++;
        } 
      }
      
      interrupt(0x10, (0x0e << 8) + '\n', 0x0, 0x0, 0x0);
      interrupt(0x10, (0x0e << 8) + '\r', 0x0, 0x0, 0x0);
    }
  }
}

void cat(char* pathReference, char currIdx) {
  int status;
  char files[1024];
  char fileBuffer[20 * 512];
  char fileIdx;

  interrupt(0x21, 2, files, 0x101, 0);
  interrupt(0x21, 2, files + 512, 0x102, 0);
  fileIdx = idxPath(pathReference, files, currIdx);

  if(*pathReference) {
    interrupt(0x21, 0x4 | (currIdx) << 8, fileBuffer, pathReference, &status);
    if (status == 0) {
      interrupt(0x21, 0x0, fileBuffer, 0x0, 0x0);
    }
    else {
      interrupt(0x21, 0x0, "Tidak dapat membaca file.", 0x0, 0x0);
    }
  }
  else {
    interrupt(0x21, 0x0, "Tidak dapat membaca file.", 0x0, 0x0);
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

void getSplittedStringOnIndex(char*target, char* str, char delimiter, int idx){
 int i;
 int wordCount = 0;
 int startIdx = 0;
 int length = lengthString(str);

 for(i = 0; i<length; i++){
   if(wordCount==idx) *(target + (i-startIdx)) = str[i];
 
   if(str[i]==delimiter){
     wordCount++;
     if(wordCount>idx){
       *(target + (i-startIdx)) = '\0';
       break;
     }
     startIdx = i+1;
   }
 }
}

int checkArgs(char* input, int argsNeeded) {
  int argsCurrent = 0, i = 0;

  while(*(input + i) != '\0') {
    if(*(input + i) == ' ' && *(input + i + 1) != ' ' && *(input + i + 1) != '\0') {
      argsCurrent++;
    }
    i++;
  }

  if(argsCurrent == argsNeeded) {
    return 1;
  }else if(argsCurrent > argsNeeded) {
    printString("To many arguments");
    return 0;
  }else{
    printString("To few arguments");
    return 0;
  }
}

void cd(char *path)
{
    char files[1024];
    char pathIdx = 0xFF;

    interrupt(0x21, 2, files, 0x101, 0);
    interrupt(0x21, 2, files + 512, 0x102, 0);

    if (*path) {
      pathIdx = idxPath(path, files, currentShellIdx);
    }

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

void mkdir(char* filename, int* flag){
  char files[1024], filenameIdx;
  int i, fileSectorIdx = -1;

  *flag = 0;

  readSector(files, 0x101);
  readSector(files + 512, 0x102);

  for(i = 0; i < 64; i++) {
    if(files[(i * 16) + 2] == 0) {
      fileSectorIdx = i;
      break;
    }
  }

  if(fileSectorIdx == -1) {
    *flag = 1;
    return;
  }

  filenameIdx = idxPath(filename, files, currentShellIdx);
  if(filenameIdx != 0x40) {
    *flag = 2;
    return;
  }else{
    files[fileSectorIdx * 16] = currentShellIdx;
    files[(fileSectorIdx * 16) + 1] = 0xFF;

    i = 0;
    while (i < 14 && filename[i] != 0){
      files[(fileSectorIdx * 16) + 2 + i] = filename[i];
      i++;
    }

    interrupt(0x21, 3, files, 0x101, 0);
    interrupt(0x21, 3, files + 512, 0x102, 0);
  }
}


void runShell()
{
  int i, flag;
  int argsNeeded = 0, argsCurrent = 0;

  char *input = "";
  char *currentDirectory = "";
  char *buffer = "";
  char args1[128], args2[128];

  while (1)
  {
    for(i = 0; i < 128; i++) {
      args1[i] = '\0';
    }

    cwd(currentShellIdx, currentDirectory);
    interrupt(0x21, 1, input, 0, 0);

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
    }
    else if(checkCommand(input, "ls")) {
      argsNeeded = 0;
      if(checkArgs(input, argsNeeded)) {
        ls(currentShellIdx);
      }
    }else if(checkCommand(input, "ln")) {
      argsCurrent = 0, argsNeeded = 2, i = 0;

      while(*(input + i) != '\0') {
        if(*(input + i) == ' ' && *(input + i + 1) != ' ' && *(input + i + 1) != '\0') {
          argsCurrent++;
        }
        i++;
      }

      if(argsCurrent == argsNeeded) {
        getSplittedStringOnIndex(args1, input, ' ', 1);
        getSplittedStringOnIndex(args2, input, ' ', 2);
        ln(args1, args2, currentShellIdx);
      }else if (argsCurrent > argsNeeded){
        printString("to many arguments");
      }else{
        printString("to few arguments");
      }
    }else if(checkCommand(input, "cat")) {
      argsCurrent = 0, argsNeeded = 1, i = 0;

      while(*(input + i) != '\0') {
        if(*(input + i) == ' ' && *(input + i + 1) != ' ' && *(input + i + 1) != '\0') {
          argsCurrent++;
        }
        i++;
      }

      if(argsCurrent == argsNeeded) {
        getSplittedStringOnIndex(args1, input, ' ', 1);
        cat(args1, currentShellIdx);
      }else if (argsCurrent > argsNeeded){
        printString("to many arguments");
      }else{
        printString("to few arguments");
      }
    }else if(checkCommand(input, "mkdir")) {
      argsCurrent = 0, argsNeeded = 1, i = 0;

      while(*(input + i) != '\0') {
        if(*(input + i) == ' ' && *(input + i + 1) != ' ' && *(input + i + 1) != '\0') {
          argsCurrent++;
        }
        i++;
      }

      if(argsCurrent == argsNeeded) {
        getSplittedStringOnIndex(args1, input, ' ', 1);
        mkdir(args1, &flag);

        if(flag == 1) {
          printString("file entry full");
        }else if(flag == 2) {
          printString("folder has been declared");
        }
      }else if (argsCurrent > argsNeeded){
        printString("to many arguments");
      }else{
        printString("to few arguments");
      }
    }else{
      interrupt(0x21, 0x0, "Command not found!", 0x0, 0x0);
    }

    interrupt(0x21, 0x0, "", 0x0, 0x0);
  }
}