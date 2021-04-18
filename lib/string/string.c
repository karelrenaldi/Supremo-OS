#include "string.h"
#include "../utils/utils.h"
#include "../definition/definition.h"

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