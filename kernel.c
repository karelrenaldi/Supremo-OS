#include "shell.h"
#include "utils.h"

#define MAX_CHAR 14
#define FALSE 0
#define TRUE 1
#define NOT_FOUND_INDEX 0x40
#define UNDEFINE_INDEX -1

// char idxPath(char *path, char *files, char parentIndex);
// char getCurrentIndex(char *name, char *files, char parentIndex);

void handleInterrupt21(int AX, int BX, int CX, int DX);
void clear(char *buffer, int length);
void drawingBox();
void drawingImage();

// shell.h
// void cd(char* path, char* currentIndex);

extern char imageFile;

int main()
{
  char *string_input = "";
  char *buffer = "";
  int flag;
  makeInterrupt21();

  // Make graphic mode
  interrupt(0x10, 0x13, 0x0, 0x0, 0x0);
  drawingImage();

  // Enter listener
  handleInterrupt21(0x1, string_input, 0x0, 0x0);

  // Back to text mode
  interrupt(0x10, 0x03, 0x0, 0x0, 0x0);

  // Print string
  handleInterrupt21(0x0, "<====== WELCOME =====>", 0x0, 0x0);
  writeFile("ini testing", "testing.txt", &flag, 0xFF);
  runShell();
}

void drawingImage()
{
  char *image = &imageFile;
  int x, y, i = 2;

  for (x = image[0]; x != 0; x--)
  {
    for (y = image[1]; y != 0; y--)
    {
      interrupt(0x10, (0xc << 8) + image[i], 0x0, x + (160 - image[0] / 2), y + (100 - image[0] / 2));
      i++;
    }
  }
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
  default:
    printString("Invalid interrupt");
  }
}