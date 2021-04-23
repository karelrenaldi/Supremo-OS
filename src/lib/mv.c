void mv(char * path1, char * path2, char *currDir);
void moveDirectory(int fromIdx, int toIdx, char * currDir, char * to, char * map, char * files, char * sector);
void moveFile(int fromIdx, int toIdx, char * currDir, char * to, char * map, char * files, char * sector);

int main() {
    char files[1024];
    char buffer[512 * 16];
    char numOfArgs[255], parentIdx[255], sourcefile[255], targetfile[255];
    int flag, idx, status;

    readSector(files, 0x101);
    readSector(files + 512, 0x102);
    
    clear(buffer, 512 * 16);
    readFile(buffer, "rpl/~msg", &flag, 0xFF);

    getSplittedStringOnIndex(numOfArgs, buffer, '-', 0);
    if(numOfArgs[0] == 3){
        getSplittedStringOnIndex(parentIdx, buffer, '-', 1);
        getSplittedStringOnIndex(sourcefile, buffer, '-', 2);
        getSplittedStringOnIndex(targetfile, buffer, '-', 3);
        mv(sourcefile, targetfile, parentIdx[0]);
    }else{
        printString("Too many arguments");
    }

    interrupt(0x21, 0x06, "shell", 0x3000, status);
    return 0;
}

void moveFile(int fromIdx, int toIdx, char * currDir, char * to, char * map, char * files, char * sector){
  int i, n, targetDirIndex, flag;
  char name[14], p[255];
  
  if (toIdx != 0x40) {
    if (files[toIdx * 16 + 1] == 0xff) {
      printString("MOVING FILE TO A EXIST FOLDER...");
      files[fromIdx * 16] = toIdx;
    } else {
      printString("Cannot move/rename file, cause same name exist!");      
    }
  } else { 
    n = lengthString(to);

    if (lengthString(to) > 14) {
      printString("Fail to rename/create directory, name cannot exceed 14 characters!");
    }

    if (contain(to,'/') == -1) { 
      for(i = 0; i < 14; i++){
        files[fromIdx*16 + 2 + i] = 0x00;
        if( i < n){
          files[fromIdx*16 + 2 + i] = to[i];
        }
      }
    } else {
      getSplittedStringOnIndex(name, to, '/', 0);
      createDir(name, &flag, currDir);
      readSector(files, 0x101);
      readSector(files+512, 0x102);

      targetDirIndex = idxPath(name, files, currDir);
      files[fromIdx * 16] = targetDirIndex;
    }
  }
}


void moveDirectory(int fromIdx, int toIdx, char * currDir, char * to, char * map, char * files, char * sector){
    int i,n, targetDirIndex, flag;
    char name[14], p[255];
    
    if (toIdx != 0x40) {
        if (files[toIdx*16 + 1] != 0xff) {
          printString("Cannot move folder to a file");
        } else { 
          files[fromIdx * 16] = toIdx;
        }
    } else { 
        n = lengthString(to);

        if (contain(to,'/') == -1) {
          for(i = 0; i < 14; i++){
              files[fromIdx*16 + 2 + i] = 0x00;
              if( i < n){
                files[fromIdx*16 + 2 + i] = to[i];
              }
          }
        } else {
          getSplittedStringOnIndex(name, to, '/', 0);
          createDir(name, &flag, currDir);
          readSector(files, 0x101);
          readSector(files+512, 0x102);

          targetDirIndex = idxPath(name, files, currDir);
          files[fromIdx * 16] = targetDirIndex;
        }
    }
}

void mv(char * path1, char * path2, char * currDir){
    char map[512];
    char files[1024];
    char sector[512];
    char p[255];
    char fromIdx, toIdx;

    readSector(map, 0x100);
    readSector(files, 0x101);
    readSector(files+512, 0x102);
    readSector(sector, 0x103);

    fromIdx = idxPath(path1, files, currDir);
    toIdx = idxPath(path2, files, currDir);

    if(fromIdx == 0x40){
      printString("FILE THAT YOU'RE GOING TO MOVE IS NOT FOUND");
      return;
    }

    if(toIdx == 0x40 && containMultiple(path2, '/') == 1){
      printString("TARGET DIRECTORY DOESN'T EXIST!");
      return;
    }

    if(fromIdx == toIdx){
      return;
    }

    if(files[fromIdx*16+1] != 0xFF){
        printString("move file");
        moveFile(fromIdx, toIdx, currDir, path2, map, files, sector);
    } else {
        printString("move folder");
        moveDirectory(fromIdx, toIdx, currDir, path2, map, files, sector);
    }

    writeSector(map, 0x100);
    writeSector(files, 0x101);
    writeSector(files + 512, 0x102);
    writeSector(sector, 0x103);
}