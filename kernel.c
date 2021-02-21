void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void clear(char *buffer, int length);

int main () {
  char *string_output = "ORANG";
  char *string_input = "";
  makeInterrupt21();
  handleInterrupt21(0x0, string_output, 0, 0);
  handleInterrupt21(0x1, string_input, 0, 0);
  handleInterrupt21(0x0, string_input, 0, 0);

  while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX) {
  switch (AX) {
    case 0x0:
      printString(BX);
      break;
    case 0x1:
      readString(BX);
      break;
    default:
      printString("Invalid interrupt");
  }
}

void printString(char *string) {
  int i = 0;
  while(string[i] != 0){
    interrupt (0x10, (0x0e << 8) + *(string + i), 0x0, 0x0, 0x0);
    i++;
  }
  interrupt (0x10, (0x0e << 8) + '\r', 0x0, 0x0, 0x0);
  interrupt (0x10, (0x0e << 8) + '\n', 0x0, 0x0, 0x0);
}

void readString(char* string) {
  int i = 0;
  char in = 0x0;
  while(in != 0x0d) {
    in = interrupt(0x16, 0x0, 0x0, 0x0, 0x0);
    *(string + i) = in;
    interrupt(0x10, (0x0e << 8) + in, 0x0, 0x0, 0x0);

    if (in == 13) interrupt(0x10, (0x0e << 8) + 10, 0x0, 0x0, 0x0);
    i++;
  }
  *(string + i) = 0x0;
}