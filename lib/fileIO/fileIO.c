#include "fileIO.h"
#include "../definition/definition.h"

void writeFile(char *buffer, char *path, int *sectorsFlag, char parentIndex)
{
  int found, text_length, sector_needed, sector_available;
  int filesSectorIdx, empty_entry_map, sectorsSectorIdx, written_sector;
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
  while (*(iterator + i))
  {
    if (*iterator == '/')
    {
      last_slash_idx = i;
    }
    i++;
  }

  // Initialization sectors
  *sectorsFlag = 0;

  // Cek file already exist or not
  filePathIndex = idxPath(path, files, parentIndex);

  if (filePathIndex != NOT_FOUND_INDEX) // Found
  {
    *sectorsFlag = -1;
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
      *sectorsFlag = -4;
      printString("Folder tidak valid");
      return;
    }
    else
    {
      fileParentIndex = filePathIndex;
    }
  }

  // Prcess if file not already exist and file directory valid
  if (*sectorsFlag != -1 && *sectorsFlag != -4)
  {
    // Find empty_entry_files
    found = FALSE;
    filesSectorIdx = 0;

    while (filesSectorIdx < 64 && !found)
    {
      if (files[(filesSectorIdx * 16) + 2] == 0)
      {
        found = TRUE;
      }
      else
      {
        filesSectorIdx++;
      }
    }

    if (!found)
    {
      *sectorsFlag = -2;
      printString("Tidak cukup entri di files");
      return;
    }
    else
    {
      // Find empty_entry_sectors
      found = FALSE;
      sectorsSectorIdx = 0;
      while (sectorsSectorIdx < 512 && !found)
      {
        if (sectors[sectorsSectorIdx * 16] == 0)
        {
          found = TRUE;
        }
        else
        {
          sectorsSectorIdx++;
        }
      }

      if (!found)
      {
        *sectorsFlag = -3;
        printString("Tidak cukup sektor kosong");
        return;
      }
      else
      {
        // fill files sector => 1 byte (parent), 1 byte (index), 14 byte(character)
        files[filesSectorIdx * 16] = fileParentIndex;
        files[(filesSectorIdx * 16) + 1] = sectorsSectorIdx;

        i = 0;
        while (i < 14 && path[last_slash_idx + 1 + i] != 0)
        {
          files[(filesSectorIdx * 16) + 2 + i] = path[last_slash_idx + 1 + i];
          i++;
        }

        sector_needed = (lengthString(buffer) / 512) + 1;
        if (sector_needed > 16)
        {
          *sectorsFlag = -3;
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
          *sectorsFlag = -3;
          printString("Tidak cukup sektor kosong");
          return;
        }
        else
        {
          printString("bisa nulis");
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
            sectors[(sectorsSectorIdx * 16) + written_sector] = i;

            written_sector++;
          }
          *sectorsFlag = written_sector;
        }
      }
    }
  }

  // Write again to sector for changes
  writeSector(map, 0x100);
  writeSector(files, 0x101);
  writeSector(files + 512, 0x102);
  writeSector(sectors, 0x103);
  printString("berhasil writefile");
}

void readFile(char *buffer, char *path, int *result, char parentIndex)
{
    char files[1024];
    char sectors[512];
    char found = FALSE;
    char fileSectorIdx;
    int j, k;
 
    readSector(files, 0x101);   
    readSector(files + 512, 0x102);
    readSector(sectors, 0x103);
 
    fileSectorIdx = idxPath(path, files, parentIndex);
    if (fileSectorIdx == NOT_FOUND_INDEX)
    {
        *result = -1;
    }
    else
    {
        if (files[(fileSectorIdx * 16) + 1] == 0xFF)
        {
            *result = -1;
        }
        else
        {
            j = files[(fileSectorIdx * 16) + 1] * 0x10;
            for (k = 0; k < 16 && sectors[j + k] != 0; k++)
            {
                readSector(buffer + (k * 512), sectors[j + k]);
            }
            *result = 0;
        }
    }
}

void removeFile(char* path, int* result, char parentIndex){
  char map[512], files[1024], sectors[512];
  char filePathIndex, sector;
  int i;

  // Baca sektor map dan dir
  readSector(map, 0x100);
  readSector(files, 0x101);
  readSector(files + 512, 0x102);
  readSector(sectors, 0x103);

  filePathIndex = idxPath(path, files, parentIndex);

  if (filePathIndex == NOT_FOUND_INDEX) // Found
  {
    printString("File tidak ada");
    return;
  }

  int i = 0;
  sector = files[filePathIndex*16 + 1];
  sector = files[filePathIndex*16 + 1];
  i = 0;

  while(i<16 && sectors[sector*16+1] != 0x00){
      map[sectors[sector*16+1]] = 0x00;
      sectors[sector*16 + 1] = 0x00;
      i++;
  }

  for(i=0; i<16; i++){
      files[filePathIndex*16+i] = 0x00;
  }

  writeSector(map, 0x100);
  writeSector(sector, 0x103);
  writeSector(files, 0x101);
  writeSector(files+512, 0x102);
}