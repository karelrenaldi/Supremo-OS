#include "shell.h"
#include "utils.h"

#define MAX_CHAR 14
#define FALSE 0
#define TRUE 1
#define NOT_FOUND_INDEX 0x40
#define UNDEFINE_INDEX -1

// math.h
int mod(int a, int m);
int div(int a, int m);

// string.h
int isSameString(char *str1, char *str2);
int lengthString(char *str);
void readString(char *string);


char idxPath(char *path, char *files, char parentIndex);
char getCurrentIndex(char *name, char *files, char parentIndex);

void handleInterrupt21(int AX, int BX, int CX, int DX);
void clear(char *buffer, int length);
void drawingBox();
void drawingImage();
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void readFile(char *buffer, char *path, int *result, char parentIndex);

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


int mod(int a, int m){
    while(a >= m){
        a -= m;
    }

    return a;
}

int div(int a, int m){
    int res = 0;

    while(res*m <=a){
        res++;
    }

    return res - 1;
}

void readString(char* string) {
   int i = 0, loop = 1;
   char input;
   while (loop) {
      input = interrupt(0x16, 0x0, 0x00, 0x00, 0x00);

      if(input == '\r') {
        string[i] = '\0';
        interrupt(0x10, (0x0E * 0x100) + '\n', 0x00, 0x00, 0x00);
        interrupt(0x10, (0x0E * 0x100) + '\r', 0x00, 0x00, 0x00);
        loop = 0;
      }else if(input == '\b') {
        if(i > 0) {
          interrupt(0x10, (0x0E * 0x100) + '\b', 0x00, 0x00, 0x00);
          interrupt(0x10, (0x0E * 0x100) + '\0', 0x00, 0x00, 0x00);
          interrupt(0x10, (0x0E * 0x100) + '\b', 0x00, 0x00, 0x00);
          i--;
        }
      }else{
        string[i] = input;
        interrupt(0x10, (0x0E * 0x100) + string[i], 0x00, 0x00, 0x00);
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

void readSector(char *buffer, int sector)
{
  int ax = (0x02 << 8) + 0x1;
  int cx = (div(sector, 36) << 8) + (mod(sector, 18) + 1);
  int dx = mod(div(sector, 18), 2) << 8;

  interrupt(0x13, ax, buffer, cx, dx);
}

void writeSector(char *buffer, int sector)
{
  int ax = (0x03 << 8) + 0x1;
  int cx = (div(sector, 36) << 8) + (mod(sector, 18) + 1);
  int dx = mod(div(sector, 18), 2) << 8;

  interrupt(0x13, ax, buffer, cx, dx);
}

void writeFile(char *buffer, char *path, int *sectorsFlag, char parentIndex)
{
  int found, text_length, sector_needed, sector_available;
  int filesSectorIdx, empty_entry_map, sectorsSectorIdx, written_sector;
  int filePathIndex, fileParentIndex, last_slash_idx, i;
  char map[512], files[1024], sectors[512], parent_path[512], current_file_data[512];
  char *iterator;

  // Baca sektor map dan dir
  readSector(map, 0x100);
  readSector(files, 0x101);
  readSector(files + 512, 0x102);
  readSector(sectors, 0x103);

  // find index of last slash
  i = 0;
  last_slash_idx = UNDEFINE_INDEX;
  iterator = path;
  while (*(iterator + i))
  {
    if (*iterator == '/')
    {
      last_slash_idx = i;
    }
    i++;
  }

  // Initialization sectors
  *sectorsFlag = 0;

  // Cek file already exist or not
  filePathIndex = idxPath(path, files, parentIndex);

  if (filePathIndex != NOT_FOUND_INDEX) // Found
  {
    *sectorsFlag = -1;
    printString("File sudah ada");
    return;
  }

  // Get file parent name path from 0 to last_slash_idx
  parent_path[0] = '\0';
  for (i = 0; i < last_slash_idx; i++)
  {
    parent_path[i] = path[i];
  }
  parent_path[i] = '\0';

  // Get parent path index
  if (parent_path[0] == '\0')
  {
    fileParentIndex = parentIndex;
  }
  else
  {
    filePathIndex = idxPath(parent_path, files, parentIndex);
    if (filePathIndex == NOT_FOUND_INDEX)
    {
      *sectorsFlag = -4;
      printString("Folder tidak valid");
      return;
    }
    else
    {
      fileParentIndex = filePathIndex;
    }
  }

  // Prcess if file not already exist and file directory valid
  if (*sectorsFlag != -1 && *sectorsFlag != -4)
  {
    // Find empty_entry_files
    found = FALSE;
    filesSectorIdx = 0;

    while (filesSectorIdx < 64 && !found)
    {
      if (files[(filesSectorIdx * 16) + 2] == 0)
      {
        found = TRUE;
      }
      else
      {
        filesSectorIdx++;
      }
    }

    if (!found)
    {
      *sectorsFlag = -2;
      printString("Tidak cukup entri di files");
      return;
    }
    else
    {
      // Find empty_entry_sectors
      found = FALSE;
      sectorsSectorIdx = 0;
      while (sectorsSectorIdx < 512 && !found)
      {
        if (sectors[sectorsSectorIdx * 16] == 0)
        {
          found = TRUE;
        }
        else
        {
          sectorsSectorIdx++;
        }
      }

      if (!found)
      {
        *sectorsFlag = -3;
        printString("Tidak cukup sektor kosong");
        return;
      }
      else
      {
        // fill files sector => 1 byte (parent), 1 byte (index), 14 byte(character)
        files[filesSectorIdx * 16] = fileParentIndex;
        files[(filesSectorIdx * 16) + 1] = sectorsSectorIdx;

        i = 0;
        while (i < 14 && path[last_slash_idx + 1 + i] != 0)
        {
          files[(filesSectorIdx * 16) + 2 + i] = path[last_slash_idx + 1 + i];
          i++;
        }

        sector_needed = (lengthString(buffer) / 512) + 1;
        if (sector_needed > 16)
        {
          *sectorsFlag = -3;
          printString("Tidak cukup sektor kosong");
          return;
        }

        // Find sector available
        sector_available = 0;
        i = 0;
        while (i < 512)
        {
          if (map[i] == 0)
          {
            sector_available++;
          }
          i++;
        }

        // Compare sector needed and sector available
        if (sector_available < sector_needed)
        {
          *sectorsFlag = -3;
          printString("Tidak cukup sektor kosong");
          return;
        }
        else
        {
          printString("bisa nulis");
          // Write to sector
          written_sector = 0;
          while (written_sector < sector_needed)
          {
            clear(current_file_data, 512);

            i = 0;
            while (i < 512 && buffer[written_sector * 512 + i] != 0)
            {
              current_file_data[i] = buffer[written_sector * 512 + i];
              i++;
            }

            // Search available sector based on map
            i = 0;
            while (i < 512 && map[i] != 0)
            {
              i++;
            }

            writeSector(current_file_data, i);
            map[i] = 0xFF; // Sektor sudah diisi
            sectors[(sectorsSectorIdx * 16) + written_sector] = i;

            written_sector++;
          }
          *sectorsFlag = written_sector;
        }
      }
    }
  }

  // Write again to sector for changes
  writeSector(map, 0x100);
  writeSector(files, 0x101);
  writeSector(files + 512, 0x102);
  writeSector(sectors, 0x103);
  printString("berhasil writefile");
}

void readFile(char *buffer, char *path, int *result, char parentIndex)
{
    char files[1024];
    char sectors[512];
    char found = FALSE;
    char fileSectorIdx;
    int j, k;
 
    readSector(files, 0x101);   
    readSector(files + 512, 0x102);
    readSector(sectors, 0x103);
 
    fileSectorIdx = idxPath(path, files, parentIndex);
    if (fileSectorIdx == NOT_FOUND_INDEX)
    {
        *result = -1;
    }
    else
    {
        if (files[(fileSectorIdx * 16) + 1] == 0xFF)
        {
            *result = -1;
        }
        else
        {
            j = files[(fileSectorIdx * 16) + 1] * 0x10;
            for (k = 0; k < 16 && sectors[j + k] != 0; k++)
            {
                readSector(buffer + (k * 512), sectors[j + k]);
            }
            *result = 0;
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