#include "utils.h"

#define MAX_CHAR 14
#define FALSE 0
#define TRUE 1
#define NOT_FOUND_INDEX 0x40
#define UNDEFINE_INDEX -1

void printString(char *string)
{
  int i = 0;
  while (string[i] != 0)
  {
    interrupt(0x10, (0x0e << 8) + string[i], 0x0, 0x0, 0x0);
    i++;
  }
  interrupt(0x10, (0x0e << 8) + '\n', 0x0, 0x0, 0x0);
  interrupt(0x10, (0x0e << 8) + '\r', 0x0, 0x0, 0x0);
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
      if (isSameString(name, files + (i * 16) + 2))
      {
        return i;
      }
    }
  }
  return NOT_FOUND_INDEX;
}