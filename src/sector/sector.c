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