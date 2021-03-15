void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void clear(char *buffer, int length);
void drawingBox();
void drawingImage();
void readSector(char *buffer, int sector);
int mod(int a, int b);
int div(int a, int b);

extern char imageFile;

int main () {
  char *string_input = "";
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

  // Loop input
  while(1) {
    handleInterrupt21(0x1, string_input, 0x0, 0x0);
  }

  while (1);
}

void drawingImage() {
  char* image = &imageFile;
  int x, y, i = 2;

  for(x = image[0]; x != 0; x--) {
    for(y = image[1]; y != 0; y--) {
      interrupt(0x10, (0xc << 8) + image[i], 0x0, x + (160 - image[0] / 2), y + (100 - image[0] / 2));
      i++;
    }
  }
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
  printString(string);
  *string = "";
}

void clear(char *buffer, int length) {
  int i;
	for (i = 0; i < length; i++) {
		buffer[i] = 0x0;
	}
}

void drawingBox() {
  int i, j;
  int x_length = 50;
  for(i = x_length; i != 0; i--) {
    int y_length = 50;
    for(j = y_length; j != 0; j--) {
      interrupt (0x10, (0x0c << 8) + 0x0c, 0x0, i + 320 - x_length - 1, j);
    }
  }
}

int mod (int a, int b){
  while(a > b){
    a -= b;
  }
  return a;
}

int div (int a, int b){
  int ans = 0;
  while(a > b){
    a -= b;
    ans++;
  }
  return ans;
}

void readSector(char *buffer, int sector){
  interrupt(0x13, 
  0x201, 
  buffer, 
  div(sector, 36) * 0x100 + mod(sector, 18) + 1, 
  mod(div(sector, 18), 2) * 0x100);
}
