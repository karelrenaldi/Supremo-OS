#include "./utils/utils.h"
#include "./string/string.h"
#include "./math/math.h"
#include "./folderIO/folderIO.h"
#include "./fileIO/fileIO.h"
#include "./definition/definition.h"
#include "./sector/sector.h"

// #define MAX_CHAR 14
// #define FALSE 0
// #define TRUE 1
// #define NOT_FOUND_INDEX 0x40
// #define UNDEFINE_INDEX -1

void handleInterrupt21(int AX, int BX, int CX, int DX);
void drawingBox();
void drawingImage();

int main()
{
  char status;
  char *string_input = "";
  char *buffer = "";
  int flag;
  makeInterrupt21();
  
  interrupt(0x10, 0x03, 0x0, 0x0, 0x0);
  interrupt(0x21, 0x06, "shell", 0x3000, status);
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
  int i, isSuccess, idx;
  char fileBuffer[512 * 16], files[1024], notfound[11], pathMSG[];

  readSector(files, 0x101);
  readSector(files + 512, 0x102);

  readFile(&fileBuffer, filename, &isSuccess, parentIndex);
  if (isSuccess == 0) {
    for (i = 0; i < 512*16; i++) {
      putInMemory(segment, i, fileBuffer[i]);
    }
    launchProgram(segment);
  } else {
    pathMSG[0] = 'r';
    pathMSG[1] = 'p';
    pathMSG[2] = 'l';
    pathMSG[3] = '/';
    pathMSG[4] = '~';
    pathMSG[5] = 'm';
    pathMSG[6] = 's';
    pathMSG[7] = 'g';
    pathMSG[8] = '\0';
    deleteFile(idxPath(pathMSG, files, 0xFF));

    clear(notfound, 11);
    notfound[0] = 'n';
    notfound[1] = 'o';
    notfound[2] = 't';
    notfound[3] = ' ';
    notfound[4] = 'f';
    notfound[5] = 'o';
    notfound[6] = 'u';
    notfound[7] = 'n';
    notfound[8] = 'd';
    notfound[9] = '\r';
    notfound[10] = '\n';
    interrupt(0x21, 0, notfound, 0,0);
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