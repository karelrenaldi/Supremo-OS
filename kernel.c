void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void clear(char *buffer, int length);
void drawing();

int main () {
  char *string_output = "Hello, World !";
  char *string_input = "";
  makeInterrupt21();
  interrupt(0x10, 0x13, 0x0, 0x0, 0x0);
  // handleInterrupt21(0x0, string_output, 0x0, 0x0);
  drawing();
  handleInterrupt21(0x1, string_input, 0x0, 0x0);
  interrupt(0x10, 0x03, 0x0, 0x0, 0x0);
  // handleInterrupt21(0x0, string_input, 0x0, 0x0);

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
  char input = 0x0;
  while(input != 0x0d) {
    input = interrupt(0x16, 0x0, 0x0, 0x0, 0x0);
    string[i] = input;
    interrupt(0x10, (0x0e << 8) + input, 0x0, 0x0, 0x0);

    if (input == 0x0d) 
      interrupt(0x10, (0x0e << 8) + 10, 0x0, 0x0, 0x0);
    i++;
  }
  string[i] = 0x0;
}

void drawing() {
  int i, j;
  int x_length = 50;
  for(i = x_length; i != 0; i--) {
    int y_length = 50;
    for(j = y_length; j != 0; j--) {
      interrupt (0x10, (0x0c << 8) + 0x0c, 0x0, i + 0xc, j + 0xc);
    }
  }
}