void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
// void readString(char *string);
void clear(char *buffer, int length);

int main () {
  char *string = "Karel";
  makeInterrupt21();
  handleInterrupt21(0, string, 0, 0);

  while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX) {
  switch (AX) {
    case 0x0:
      printString(BX);
      break;
    // case 0x1:
    //   readString(BX);
    //   break;
    default:
      printString("Invalid interrupt");
  }
}

void printString(char *string) {
  int i;
  while(*(string + i) != '\0'){
    interrupt (0x10, (0x0e << 8) + *(string + i), 0, 0, 0);
    i++;
  }
}