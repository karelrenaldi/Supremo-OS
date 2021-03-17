#include "shell.h"

void ln(char* pathReference, char* filename, char currIdx) {
  char files[1024];
  char pathReferenceIdx, filenameIdx;
  int i, emptyIdx;

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
  int i, idx = 0, neff = 0;
  char pathIndexArray[128], files[1024];
  char currentIndex = currIdx;
  char currentParentIndex;

  if(currIdx == 0xFF) {
    currentDirectory[idx++] = '/';
  }else{
    currentDirectory[idx++] = '/';

    interrupt(0x21, 2, files, 0x101, 0);
    interrupt(0x21, 2, files + 512, 0x102, 0);

    while((unsigned char)currentIndex != 0xFF) {
      pathIndexArray[neff] = currentIndex;
      currentParentIndex = files[currentIndex * 16];
      currentIndex =  currentParentIndex;
      neff++;
    }

    i = neff - 1;
    while(i >= 0){
      while(files[(pathIndexArray[i] * 16) + 2 + i] != '\0') {
        currentDirectory[idx++] = files[(pathIndexArray[i] * 16) + 2 + i];
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
  char fileBuffer[20 * 512];
  if (*pathReference) {
    interrupt(0x21, 0x4 | (currIdx) << 8, fileBuffer, pathReference, &status);
    if (status == 0) {
      interrupt(0x21, 0x0, fileBuffer, 0x0, 0x0);
      interrupt(0x21, 0x0, "\r\n", 0x0, 0x0);
    }
    else {
      interrupt(0x21, 0x0, "Tidak dapat membaca file.\r\n", 0x0, 0x0);
    }
  }
  else {
    interrupt(0x21, 0x0, "Tidak dapat membaca file. \r\n", 0x0, 0x0);
  }
}

void runShell()
{
  int i = 0;
  int flag;
  char *string_input = "";
  char *currentDirectory = "";
  char *buffer = "";
  char currentShellIdx = 0xFF;

  ln("iseng.txt", "iseng2.txt", currentShellIdx);
  while (1)
  {
    cwd(currentShellIdx, currentDirectory);
    interrupt(0x21, 1, string_input, 0, 0);
    readFile(buffer, "iseng.txt", &flag, 0xFF);
    ls(currentShellIdx);
    printString(buffer);
  }
}