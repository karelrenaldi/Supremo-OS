void cp(char* sourcePath, char* destinationPath);

int main() {
    char files[1024];
    char buffer[512 * 16];
    char numOfArgs[255], parentIdx[255], sourcefile[255], targetfile[255];
    int flag, idx, status;

    readSector(files, 0x101);
    readSector(files + 512, 0x102);
    
    clear(buffer, 512 * 16);
    readFile(buffer, "rpl/~msg", &flag, 0xFF);
    printString(buffer);

    getSplittedStringOnIndex(numOfArgs, buffer, '-', 0);
    if(numOfArgs[0] == 3){
        getSplittedStringOnIndex(parentIdx, buffer, '-', 1);
        getSplittedStringOnIndex(sourcefile, buffer, '-', 2);
        getSplittedStringOnIndex(targetfile, buffer, '-', 3);
        cp(sourcefile, targetfile, parentIdx[0]);
    }else{
        printString("Too many arguments");
    }

    interrupt(0x21, 0x06, "shell", 0x3000, status);
    return 0;
}

void cp(char* sourcePath, char* destinationPath, char currentShellIdx) {
  char files[1024];
  char fileBuffer[20 * 512];
  char filename[14];
  char sourcePathIdx, destinationPathIdx;
  int status, i = 0, j = 0, x, y = 0;

  while(i < 14 && destinationPath[i] != 0) {
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
    createDir(filename, &status, currentShellIdx);
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