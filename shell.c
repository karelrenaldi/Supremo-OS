#include "shell.h"

void ln(char* pathReference, char* filename, char currIdx) {
  char files[1024];
  char pathReferenceIdx;
  int i, emptyIdx, filenameIdx;

  interrupt(0x21, 2, files, 0x101, 0);
  interrupt(0x21, 2, files + 512, 0x102, 0);

  pathReferenceIdx = idxPath(pathReference, files, currIdx);

  if(idxPath == 0xFF) {
    printString("Tidak bisa ln folder");
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
  printString(currentDirectory);
  // interrupt(0x21, 0, currentDirectory, 0, 0);
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

void runShell()
{
  char *currentDirectory = "";
  cwd(0xFF, currentDirectory);
  ln("iseng.txt", "hoho.txt", 0xFF);
  ls(0xFF);
}