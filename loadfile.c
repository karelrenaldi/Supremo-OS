#include <stdio.h>

int main(int argc, char* argv[]){
    int idx;
    int fileIdx;
    int countSector = 0;

    // Check argc
    if(argc < 2){
        printf("Specify a file to load\n");
        return;
    }

    // Open the file (r mode)
    FILE* loadFile;
    loadFile = fopen(argv[1], "r");

    // File not found
    if(loadFile == 0){
        printf("File not found\n");
        return;
    }
    
    // Open system image (r+ mode)
    FILE* system;
    system = fopen("system.img", "r+");

    // System not found
    if(system == 0){
        printf("system.img not found\n");
        return;
    }

    // Load map
    char map[512];
    fseek(system, 512 * 0x100, SEEK_SET);
    
    for(idx = 0; idx < 512; idx++){
        map[idx] = fgetc(system);
    }

    // Load file list
    char files[1024];
    fseek(system, 512 * 0x101, SEEK_SET);
    
    for(idx = 0; idx < 1024; idx++){
        files[idx] = fgetc(system);
    }
    
    // Load sector list
    char sector[512];
    fseek(system, 512 * 0x103, SEEK_SET);
    
    for(idx = 0; idx < 512; idx++){
     sector[idx] = fgetc(system);
    }

    // Search for a free entry in files
    for(idx = 0; idx < 1024; idx += 16){
        if(files[idx + 2] == 0){break;}
    }

    // Check if entry is found
    if(idx == 1024){
        printf("Not enough room in files\n");
        return;
    }

    // Set dirIdx to idx of entry
    fileIdx = idx;

    // Fill 00s to name field
    for(idx = 0; idx < 14; idx++){
        files[idx + fileIdx + 2] = 0x00;
    }

    // Copy the name to the field
    for(idx = 0; idx < 14; idx++){
        if(argv[2][idx] == 0){break;}
        files[fileIdx + idx + 2] = argv[2][idx];
    }
    
    // Find free entry in sectors
    for(idx = 0; idx < 32; idx++){
     if(sector[idx * 16] == 0){break;}
    }
    
    // Check if entry is found
    if(idx == 32){
     printf("Not enough room in sectors\n");
     return;
    }
    
    int idxSect = idx * 16;
    files[fileIdx + 1] = idx;
    files[fileIdx] = 0xFF;

    // Search for free sectors
    // Add these sectors to the file

    while(!feof(loadFile)){
        if(countSector == 16){
            printf("Not enough space in directory entry for the file\n");
            return;
        }

        // Search for a free entry in map
        for(idx = 0; idx < 256; idx++){
            if(map[idx] == 0){break;}
        }

        // Check if entry is found
        if(idx == 256){
            printf("Not enough room for the file\n");
            return;
        }

        // Mark the entry as used
        map[idx] = 0xFF;

        // Mark the sector in directory
        sector[idxSect] = idx;
        idxSect++;
        countSector++;

        printf("Loaded %s to sector %d\n", argv[2], idx);

        // Move and write the sector
        fseek(system, idx * 512, SEEK_SET);
        for(idx = 0; idx < 512; idx++){
            if(feof(loadFile)){
                fputc(0x0, system);
                break;
            }
            else{
                char newChar = fgetc(loadFile);
                fputc(newChar, system);
            }
        }
    }

    // Write map and directory to system
    fseek(system, 512 * 0x100, SEEK_SET);

    for(idx = 0; idx < 512; idx++){
        fputc(map[idx], system);
    }
    
    fseek(system, 512 * 0x101, SEEK_SET);

    for(idx = 0; idx < 1024; idx++){
        fputc(files[idx], system);
    }

    fseek(system, 512 * 0x103, SEEK_SET);
    
    for(idx = 0; idx < 512 * 2; idx++){
        fputc(sector[idx], system);
    }

    // Close the opened files
    fclose(loadFile);
    fclose(system);

    return 0;
}

