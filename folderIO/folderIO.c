#include "folderIO.h"
#include "../sector/sector.h"

void createDir(char* filename, int* flag, char currentShellIdx){
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