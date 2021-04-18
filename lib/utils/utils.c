#include "utils.h"
#include "../definition/definition.h"
#include "../string/string.h"

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