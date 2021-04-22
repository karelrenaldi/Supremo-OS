#include "shell.h"
#include "../utils/utils.h"
#include "../definition/definition.h"

char currentShellIdx = 0xFF;

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

void moveFile(int fromIdx, int toIdx, char * currDir, char * to, char * map, char * files, char * sector){
  int i, n, targetDirIndex, flag;
  char name[14], p[255];
  
  if (toIdx != NOT_FOUND_INDEX) {
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
      mkdir(name, &flag);
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
    
    if (toIdx != NOT_FOUND_INDEX) {
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
          mkdir(name, &flag);
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

    if(fromIdx == NOT_FOUND_INDEX){
      printString("FILE THAT YOU'RE GOING TO MOVE IS NOT FOUND");
      return;
    }

    if(toIdx == NOT_FOUND_INDEX && containMultiple(path2, '/') == 1){
      printString("TARGET DIRECTORY DOESN'T EXIST!");
      return;
    }

    if(fromIdx == toIdx){
      return;
    }

    if(files[fromIdx*16+1] != 0xFF){
        printString("MOVING FILE...");
        moveFile(fromIdx, toIdx, currDir, path2, map, files, sector);
    } else {
        printString("MOVING FOLDER...");
        moveDirectory(fromIdx, toIdx, currDir, path2, map, files, sector);
    }

    writeSector(map, 0x100);
    writeSector(files, 0x101);
    writeSector(files + 512, 0x102);
    writeSector(sector, 0x103);
}


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
  }else if(pathReferenceIdx == 0x40){
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

void cp(char* sourcePath, char* destinationPath) {
  char files[1024];
  char fileBuffer[20 * 512];
  char filename[MAX_CHAR];
  char sourcePathIdx, destinationPathIdx;
  int status, i = 0, j = 0, x, y = 0;

  while(i < MAX_CHAR && destinationPath[i] != 0) {
    if(destinationPath[i] == '/'){
      j = i + 1;
    }
    i++;
  }
  

  for(x = j; x < i; x++) {
    filename[y] = destinationPath[x];
    y++;
  }

  filename[y] = '\0';


  interrupt(0x21, 2, files, 0x101, 0);
  interrupt(0x21, 2, files + 512, 0x102, 0);

  sourcePathIdx = idxPath(sourcePath, files, currentShellIdx);
  if(files[(sourcePathIdx*16) + 1] == 0xff) {
    mkdir(filename, &status);
    interrupt(0x21, 0x0, "Copy folder!", 0x0, 0x0);
    return;
  }

  interrupt(0x21, 0x4 | (currentShellIdx) << 8, fileBuffer, sourcePath, &status);
  if(status == 0) {
    destinationPathIdx = idxPath(destinationPath, files, currentShellIdx);
    if(destinationPathIdx != 0x40) {
      interrupt(0x21, 0x0, "File sudah ada", 0x0, 0x0);
    }else{
      writeFile(fileBuffer, destinationPath, &status, currentShellIdx);
      interrupt(0x21, 0x0, "Berhasil copy file", 0x0, 0x0);
    }
  }else{
    interrupt(0x21, 0x0, "File tidak ditemukan.", 0x0, 0x0);
  }
}

void deleteFile(int fileIdx, char * map, char * files, char * sector){
  int sectorIdx;
  int i;
  
  sectorIdx = files[fileIdx*16 + 1];
  for(i = 0; i<16; i++){
    if(sector[sectorIdx*16 +i]!=0x00 && sectorIdx!=0xFF){
      map[sector[sectorIdx*16 +i]] = 0x00;
      sector[sectorIdx+i] = 0x00;
    }
    files[fileIdx*16 + i] = 0x00;
  }
  writeSector(map, 0x100);
  writeSector(files, 0x101);
  writeSector(files + 512, 0x102);
  writeSector(sector, 0x103);
}


void deleteDirectory(int folderIdx, char * map, char * files, char * sector){
  int i;
  int sectorIdx;

  sectorIdx = files[folderIdx*16 + 1];

  for(i = 0; i<64; i++){
    if(files[i*16]==folderIdx){
      if(files[i*16+1]==0xff){
        deleteDirectory(i, map, files, sector);
      }else{
        deleteFile(i, map, files, sector);
      }
    }
  }
  deleteFile(folderIdx, map, files, sector);
}

int getIdxFromFile(char * filename, char * dir, char * files){
  int i,j,found;
  found = -1;
  for(i = 0; i<64; i++){
    if(files[i*16]==dir){
      for(j = 2; j<16; j++){
        found = 1;
        if(filename[j-2]!=files[i*16+j]){
          found = -1;
          break;
        }
      }
    }
  }
}

void rm(char *name, char *currDir) {
  char map[512];
  char files[1024];
  char sector[512];
  int targetFileIdx;

  readSector(map, 0x100);
  readSector(files, 0x101);
  readSector(files+512, 0x102);
  readSector(sector, 0x103);

  targetFileIdx = getIdxFromFile(name, currDir, files);
  if(targetFileIdx==-1){
      printString("File not found!");
      return;
  }

  if(files[targetFileIdx*16 + 1]==0xff){
    printString("Deleting folder...");
    deleteDirectory(targetFileIdx,map, files, sector);
  }else{
    printString("Deleting File...");
    deleteFile(targetFileIdx, map, files, sector);
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

  // writeFile("ini testing", "testing3.txt", &flag, 0xFF);

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
    }else if(checkCommand(input, "cp")) {
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
        cp(args1, args2);
      }else if (argsCurrent > argsNeeded){
        printString("to many arguments");
      }else{
        printString("to few arguments");
      }
    }else if(checkCommand(input, "rm")) {
      argsCurrent = 0, argsNeeded = 1, i = 0;

      while(*(input + i) != '\0') {
        if(*(input + i) == ' ' && *(input + i + 1) != ' ' && *(input + i + 1) != '\0') {
          argsCurrent++;
        }
        i++;
      }

      if(argsCurrent == argsNeeded) {
        getSplittedStringOnIndex(args1, input, ' ', 1);
        rm(args1, currentShellIdx);
      }else if (argsCurrent > argsNeeded){
        printString("to many arguments");
      }else{
        printString("to few arguments");
      }
    }else if(checkCommand(input, "mv")) {
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
        mv(args1, args2, currentShellIdx);
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