int mod(int a, int m);
int div(int a, int m);

void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void clear(char *buffer, int length);
void drawingBox();
void drawingImage();
void readSector(char *buffer, int sector);
void writeFile(char* buffer, char* path, int *sectors, char parentIndex);
void readFile(char* buffer, char* path, int *result, char parentIndex);

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

int mod(int a, int m) {
  int res = a;

  while(res >= m) {
    res = res - m;
  }

  return res;
}

int div(int a, int m) {
  int res = 0;

  while(res * m <= a) {
    res += 1;
  }

  return res;
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

void readSector(char *buffer, int sector) {
  int ax = (0x02 << 8) + 0x1;
  int cx = (div(sector, 36) << 8) + (mod(sector, 18) + 1);
  int dx = mod(div(sector, 18), 2) << 8;

  interrupt(0x13, ax, buffer, cx, dx);
}

void writeSector(char* buffer, int sector) {
  int ax = (0x03 << 8) + 0x1;
  int cx = (div(sector, 36) << 8) + (mod(sector, 18) + 1);
  int dx = mod(div(sector, 18), 2) << 8;

  interrupt(0x13, ax, buffer, cx, dx);
}

void writeFile(char* buffer, char* path, int *sectors, char parentIndex) {
  int empty_entry, found, empty_map_sector, text_length, sector_needed;
  char map[512], files[1024];
  char* iterator;

  // Baca sektor map dan dir
  readSector(map, 0x100);
  readSector(files, 0x101);
  readSector(files + 512, 0x102);

  // Cek direktori yang kosong
  found = 0;
  empty_entry = 0;
  while(empty_entry < 32 && found == 0) {
    if(files[empty_entry * 32] == 0x00) {
      found = 1;
    }else{
      empty_entry++;
    }
  }

  if(found != 1) {
    printString("Tidak ditemukan entry kosong");
  }else{
    // Count sector_needed
    text_length = 0;
    iterator = buffer;
    while(*iterator) {
      iterator++;
      text_length++;
    }
    sector_needed = div(text_length, 512) + 1;

    // Count available sector
    
  }
}

void readFile(char* buffer, char* path, int *result, char parentIndex);

void handleInterrupt21 (int AX, int BX, int CX, int DX) {
  char AL, AH;
  AL = (char) (AX);
  AH = (char) (AX >> 8);
  switch (AL) {
    case 0x00:
      printString(BX);
      break;
    case 0x01:
      readString(BX);
      break;
    case 0x02:
      readSector(BX, CX);
      break;
    default:
      printString("Invalid interrupt");
  }
}
