#include "folderIO.h"
#include "../fileIO/fileIO.h"
#include "../sector/sector.h"

void removeDirectory(int folderIdx) {
  int sectorIdx;
  int i,j;
  char map[512], files[1024], sector[512];

  readSector(map, 0x100);
  readSector(files, 0x101);
  readSector(files + 512, 0x102);
  readSector(sector, 0x103);

  // delete folder
  sectorIdx = files[folderIdx*16 + 1];
  for(i = 0; i<16; i++){
    if(sector[sectorIdx*16 +i]!=0x00 && sectorIdx!=0xFF){
      map[sector[sectorIdx*16 +i]] = 0x00;
      sector[sectorIdx*16+i] = 0x00;
    }
    files[folderIdx*16 + i] = 0x00;
  }

  // delete files yg ada di folder
  for (i = 0; i < 64; i++){
    j = 0;
    if(files[i*16] == folderIdx){
      deleteFile(i);
    }
  }

  writeSector(map, 0x100);
  writeSector(files, 0x101);
  writeSector(files + 512, 0x102);
  writeSector(sector, 0x103);
}

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

// basically ls
void listFile(char currentIndex){
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