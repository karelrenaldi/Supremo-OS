#include "shell/shell.h"
#include "utils/utils.h"

#define MAX_CHAR 14
#define FALSE 0
#define TRUE 1
#define NOT_FOUND_INDEX 0x40
#define UNDEFINE_INDEX -1


void handleInterrupt21(int AX, int BX, int CX, int DX);
void clear(char *buffer, int length);
void drawingBox();
void drawingImage();

int main()
{
  char status;
  char *string_input = "";
  char *buffer = "";
  int flag;
  makeInterrupt21();
  
  // Back to text mode
  interrupt(0x10, 0x03, 0x0, 0x0, 0x0);

  // Print string
  handleInterrupt21(0x0, "<====== WELCOME =====>", 0x0, 0x0);
  interrupt(0x21, 0x06, "ls", 0x3000, status);
  // runShell();
  // while(1);
}

void clear(char *buffer, int length)
{
  int i;
  for (i = 0; i < length; i++)
  {
    buffer[i] = 0x0;
  }
}

void drawingBox()
{
  int i, j;
  int x_length = 50;
  for (i = x_length; i != 0; i--)
  {
    int y_length = 50;
    for (j = y_length; j != 0; j--)
    {
      interrupt(0x10, (0x0c << 8) + 0x0c, 0x0, i + 320 - x_length - 1, j);
    }
  }
}

void executeProgram(char *filename, int segment, int *success, char parentIndex) {
  // Buat buffer
  int i;
  int isSuccess;
  char fileBuffer[512 * 16];
  // Buka file dengan readFile
  printString(filename);
  readFile(&fileBuffer, "iseng.txt", &isSuccess, 0xFF);
  // If success, salin dengan putInMemory
  if (isSuccess) {
    // launchProgram
    for (i = 0; i < 512*16; i++) {
      putInMemory(segment, i, fileBuffer[i]);
    }
    launchProgram(segment);
  } else {
    interrupt(0x21, 0, "File not found!", 0,0);
  }
}


void handleInterrupt21(int AX, int BX, int CX, int DX)
{
  char AL, AH;
  AL = (char)(AX);
  AH = (char)(AX >> 8);
  switch (AL)
  {
  case 0x00:
    printString(BX);
    break;
  case 0x01:
    readString(BX);
    break;
  case 0x02:
    readSector(BX, CX);
    break;
  case 0x03:
    writeSector(BX, CX);
    break;
  case 0x04:
    readFile(BX, CX, DX, AH);
    break;
  case 0x05:
    writeFile(BX, CX, DX, AH);
    break;
  case 0x06:
    executeProgram(BX, CX, DX, AH);
    break;
  default:
    printString("Invalid interrupt");
  }
}