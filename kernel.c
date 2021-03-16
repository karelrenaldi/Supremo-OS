#include "shell.h"

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
void printString(char *string);
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

extern char imageFile;

int main()
{
  char *string_input = "";
  char *buffer = "";
  int flag;
  makeInterrupt21();

  // Make graphic mode
  // interrupt(0x10, 0x13, 0x0, 0x0, 0x0);
  // drawingImage();

  // Enter listener
  // handleInterrupt21(0x1, string_input, 0x0, 0x0);

  // Back to text mode
  interrupt(0x10, 0x03, 0x0, 0x0, 0x0);

  // Print string
  handleInterrupt21(0x0, "<====== WELCOME =====>", 0x0, 0x0);
  runShell();
  // printString("anjay anjay");
  // readFile(buffer, "coba.txt", &flag, 0xFF);
  // printString(buffer);
  // cwd(0xFF, buffer);


  // Loop input
  while (1)
  {
    handleInterrupt21(0x1, string_input, 0x0, 0x0);
  }
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


void printString(char *string)
{
  int i = 0;
  while (string[i] != 0)
  {
    interrupt(0x10, (0x0e << 8) + *(string + i), 0x0, 0x0, 0x0);
    i++;
  }
  interrupt(0x10, (0x0e << 8) + '\r', 0x0, 0x0, 0x0);
  interrupt(0x10, (0x0e << 8) + '\n', 0x0, 0x0, 0x0);
}

void readString(char *string)
{
  int i = 0;
  char input = 0x0;
  while (input != 0x0d)
  {
    input = interrupt(0x16, 0x0, 0x0, 0x0, 0x0);
    string[i] = input;
    interrupt(0x10, (0x0e << 8) + input, 0x0, 0x0, 0x0);

    if (input == 0x0d)
      interrupt(0x10, (0x0e << 8) + 10, 0x0, 0x0, 0x0);

    i++;
  }
  string[i] = 0x0;
  printString(string);
  *string = "";
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

int lengthString(char *str)
{
  int i = 0;
  while (str[i] != 0)
  {
    i++;
  }
  return i;
}

int isSameString(char *str1, char *str2)
{
  int i;
  int lengthString1 = lengthString(str1);
  int lengthString2 = lengthString(str2);

  if (lengthString1 == lengthString2)
  {
    for (i = 0; i < lengthString1; i++)
    {
      if (str1[i] != str2[i])
      {
        return FALSE;
      }
    }
    return TRUE;
  }

  return FALSE;
}

char idxPath(char *path, char *files, char parentIndex)
{
  int i = 0, j;
  char currentDirName[MAX_CHAR], currentIndex;

  while (i < MAX_CHAR && path[i] != '/' && path[i] != 0)
  {
    currentDirName[i] = path[i];
    i++;
  }


  currentDirName[i] = '\0';
  currentIndex = getCurrentIndex(currentDirName, files, parentIndex);

  if (path[i] == '\0')
  {
    return currentIndex;
  }
  else if (currentIndex == NOT_FOUND_INDEX)
  {
    return NOT_FOUND_INDEX;
  }
  else
  {
    return idxPath(path + i + 1, files, currentIndex);
  }
}

char getCurrentIndex(char *name, char *files, char parentIndex)
{
  int i;

  if (name[0] == '.' && name[1] == 0)
  {
    return parentIndex;
  }
  else if (name[0] == '.' && name[1] == '.' && name[2] == 0)
  {
    if (parentIndex == 0xFF)
    { // Root folder
      return parentIndex;
    }
    else
    {
      return files[parentIndex * 16];
    }
  }

  for (i = 0; i < 64; i++)
  {
    if (files[i * 16] == parentIndex)
    {
      printString("Masuk satu");
      if (isSameString(name, files + (i * 16) + 2))
      {
        printString("Masuk dua");
        return i;
      }
    }
  }
  return NOT_FOUND_INDEX;
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex)
{
  int found, text_length, sector_needed, sector_available;
  int empty_entry_files, empty_entry_map, empty_entry_sectors, written_sector;
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
  while (*iterator)
  {
    if (*iterator == '/')
    {
      last_slash_idx = i;
    }
    i++;
  }

  // Initialization sectors
  *sectors = 0;

  // Cek file already exist or not
  filePathIndex = idxPath(path, files, parentIndex);

  if (filePathIndex != NOT_FOUND_INDEX) // Found
  {
    *sectors = -1;
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
      *sectors = -4;
      printString("Folder tidak valid");
      return;
    }
    else
    {
      fileParentIndex = filePathIndex;
    }
  }

  // Prcess if file not already exist and file directory valid
  if (*sectors != -1 && *sectors != -4)
  {
    // Find empty_entry_files
    found = FALSE;
    empty_entry_files = 0;

    while (empty_entry_files < 64 && !found)
    {
      if (files[(empty_entry_files * 16) + 2] == 0)
      {
        found = TRUE;
      }
      else
      {
        empty_entry_files++;
      }
    }

    if (!found)
    {
      *sectors = -2;
      printString("Tidak cukup entri di files");
      return;
    }
    else
    {
      // Find empty_entry_sectors
      found = FALSE;
      empty_entry_sectors = 0;
      while (empty_entry_sectors < 32 && !found)
      {
        if (sectors[empty_entry_sectors * 16] == 0)
        {
          found = TRUE;
        }
        else
        {
          empty_entry_sectors++;
        }
      }

      if (!found)
      {
        *sectors = -3;
        printString("Tidak cukup sektor kosong");
        return;
      }
      else
      {
        // fill files sector => 1 byte (parent), 1 byte (index), 14 byte(character)
        files[empty_entry_files * 16] = fileParentIndex;
        files[(empty_entry_files * 16) + 1] = empty_entry_sectors;

        i = 0;
        while (i < 14 && path[last_slash_idx + 1 + i] != 0)
        {
          files[(empty_entry_files * 16) + 2 + i] = path[last_slash_idx + 1 + i];
          i++;
        }

        while (i < 14)
        {
          files[(empty_entry_files * 16) + 2 + i] = '\0';
          i++;
        }

        sector_needed = div(lengthString(buffer), 512) + 1;
        if (sector_needed > 16)
        {
          *sectors = -3;
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
          *sectors = -3;
          printString("Tidak cukup sektor kosong");
          return;
        }
        else
        {
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
            sectors[(empty_entry_sectors * 16) + written_sector] = i;

            written_sector++;
          }
          *sectors = written_sector;
        }
      }
    }
  }

  // Write again to sector for changes
  writeSector(map, 0x100);
  writeSector(files, 0x101);
  writeSector(files + 512, 0x102);
  writeSector(sectors, 0x103);
}

void readFile(char *buffer, char *path, int *result, char parentIndex)
{
  int i;
  int entry_sector_idx;

  char files[1024];
  char sectors[512];
  char iseng[512];
  char currentIndex;

  readSector(files, 0x101);
  readSector(files + 512, 0x102);

  currentIndex = idxPath(path, files, parentIndex);
  printString("dapat");
  if (currentIndex != NOT_FOUND_INDEX)
  {
    entry_sector_idx = files[(currentIndex * 16) + 1];

    // Check folder or file
    if (entry_sector_idx == 0xff)
    {
      *result = -1;
      printString("File tidak ditemukan, Path ini adalah folder!");
      return;
    }
    else
    {
      readSector(sectors, 0x103);
      i = 0;

      while (i < 16 && sectors[entry_sector_idx + i] != 0)
      {
        clear(iseng, 512);
        readSector(buffer + (i * 512), sectors[entry_sector_idx + i]);
        readSector(iseng, sectors[entry_sector_idx + i]);
        i++;
      }
      *result = 0;
    }
  }
  else
  {
    *result = -1;
    printString("File tidak ditemukan");
    return;
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