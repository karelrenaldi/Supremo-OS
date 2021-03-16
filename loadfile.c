#include <stdio.h>
#include <string.h>
 
 
char getDirIndex(char* name, char* dir, char parentIndex) {
 
    int i;
    if (!strcmp(name, ".")) {
        return parentIndex;
    } 
    if (!strcmp(name, "..")) {
        if (parentIndex == 0xFF)
            return 0xFF;
        return dir[parentIndex * 0x10];
    }
    for (i = 0; i < 1024; i = i + 0x10) {
        if (dir[i] == parentIndex) {
            if (!strcmp(name, dir + (i + 2)) || !strncmp(name, dir + (i + 2), 12)) {
                return i / 0x10;
            }
        }
    } 
    return 0xFE; // not found
    
}
 
char getPathIndex(char* path, char* dir, char parentIndex) {
    int i, j, finalDir;
    char searchIndex;
    char temp[15];
    finalDir = 0;
 
    for (i = 0; i < 14 && path[i] != '/' && path[i] != '\0'; i++) {
        temp[i] = path[i];
    }
    if (path[i] == '\0')
        finalDir = 1;
    temp[i] = 0;
    j = i + 1;
    searchIndex = getDirIndex(temp, dir, parentIndex);
    if (searchIndex == 0xFE) {
        return 0xFE;
    }
    if (finalDir) {
        return searchIndex;
    }
    return getPathIndex(path + j, dir, searchIndex);
}
 
void main(int argc, char* argv[]) {
  int i;
 
  if (argc < 2) {
    printf("Specify file name to load\n");
    return;
  }
 
  // open the source file
  FILE* loadFil;
  loadFil = fopen(argv[1], "r");
  if (loadFil == 0) {
    printf("File not found\n");
    return;
  }
 
  // open the floppy image
  FILE* floppy;
  floppy = fopen("system.img", "r+");
  if (floppy == 0) {
    printf("system.img not found\n");
    return;
  }
 
  // load the disk map
  char map[512];
  fseek(floppy, 512 * 0x100, SEEK_SET);
  for (i = 0; i < 512; i++) map[i] = fgetc(floppy);
 
  // load the directory
  char dir[1024];
  fseek(floppy, 512 * 0x101, SEEK_SET);
  for (i = 0; i < 1024; i++) dir[i] = fgetc(floppy);
 
  // load the file sector indices
  char files[512];
  fseek(floppy, 512 * 0x103, SEEK_SET);
  for (i = 0; i < 512; i++) files[i] = fgetc(floppy);
  
  // find a free entry in the directory
  for (i = 0; i < 1024; i = i + 0x10)
    if (dir[i] == 0 && dir[i + 1] == 0 && dir[i + 2] == 0) break;
  if (i == 1024) {
    printf("Not enough room in directory\n");
    return;
  }
  int dirindex = i;
 
  // find a empty file entry in files
  for (i = 0; i < 512; i = i + 0x10)
    if (files[i] == 0) break;
  if (i == 512) {
    printf("Not enough room in file entry list\n");
    return;
  }
  int fileindex = i;
 
 
  if (argc > 2) { 
    char parentindex = getPathIndex(argv[2], dir, 0xFF);
    if (parentindex == 0xFE) {
      printf("Path not found");
      return;
    }
    dir[dirindex] = (char) (parentindex);
  } else {
    // set parent index with 0xFF;
    dir[dirindex] = 0xFF;
  }
  // set file index
  dir[dirindex + 1] = fileindex / 0x10;
  // fill the name field with 00s first
  for (i = 0; i < 14; i++) dir[dirindex + 2 + i] = 0x00;
  // copy the name over
  for (i = 0; i < 14; i++) {
    if (argv[1][i] == 0) break;
    dir[dirindex + 2 + i] = argv[1][i];
  } 
 
  // find free sectors and add them to the file
  int sectcount = 0;
  while (!feof(loadFil)) {
    if (sectcount == 20) {
      printf("Not enough space in directory entry for file\n");
      return;
    }
 
    // find a free map entry
    for (i = 0; i < 256; i++)
      if (map[i] == 0) break;
    if (i == 256) {
      printf("Not enough room for file\n");
      return;
    }
 
    // mark the map entry as taken
    map[i] = 0xFF;
    // mark the sector in the directory entry
    files[fileindex] = i;
    fileindex++;
    sectcount++;
 
    printf("Loaded %s to sector %d\n", argv[1], i);
 
    // move to the sector and write to it
    fseek(floppy, i * 512, SEEK_SET);
    for (i = 0; i < 512; i++) {
      if (feof(loadFil)) {
        fputc(0x0, floppy);
        break;
      } else {
        char c = fgetc(loadFil);
        fputc(c, floppy);
      }
    }
  }
 
  // write the map and directory back to the floppy image
  fseek(floppy, 512 * 0x100, SEEK_SET);
  for (i = 0; i < 512; i++) fputc(map[i], floppy);
 
  fseek(floppy, 512 * 0x101, SEEK_SET);
  for (i = 0; i < 1024; i++) fputc(dir[i], floppy);
 
 
  fseek(floppy, 512 * 0x103, SEEK_SET);
  for (i = 0; i < 512; i++) fputc(files[i], floppy);
 
  fclose(floppy);
  fclose(loadFil);
}
 
 

