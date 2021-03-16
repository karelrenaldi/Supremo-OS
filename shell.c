#include "shell.h";

void runShell()
{
    interrupt(0x21, 0, "Hello From Shell", 0, 0);
}

void cwd(char parentIndex, char* buffer) {
  int neff = 0;
  int i;
  char pathIndexArray[128], files[1024];
  char currentIndex = parentIndex;
  char currentParentIndex;

  printString("Hello this is from cwd");
  if(parentIndex == 0xFF) {
    interrupt(0x10, (0x0e << 8) + '/', 0x0, 0x0, 0x0);
  }else{
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
        interrupt(0x10, (0x0e << 8) + files[(pathIndexArray[i] * 16) + 2 + i], 0x0, 0x0, 0x0);
      }
      interrupt(0x10, (0x0e << 8) + '/', 0x0, 0x0, 0x0);
      i--;
    }
  }
  interrupt(0x10, (0x0e << 8) + '$', 0x0, 0x0, 0x0);
  interrupt(0x10, (0x0e << 8) + ' ', 0x0, 0x0, 0x0);
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
    }
  }
}