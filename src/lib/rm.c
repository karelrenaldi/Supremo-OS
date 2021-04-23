void rm(char *name, char *currDir);

int main() {
    char files[1024];
    char buffer[512 * 16];
    char numOfArgs[255];
    char parentIdx[255];
    char filename[255];
    int flag, idx, status;

    readSector(files, 0x101);
    readSector(files + 512, 0x102);
    
    clear(buffer, 512 * 16);
    readFile(buffer, "rpl/~msg", &flag, 0xFF);

    getSplittedStringOnIndex(numOfArgs, buffer, '-', 0);
    if(numOfArgs[0] == 2){
        getSplittedStringOnIndex(parentIdx, buffer, '-', 1);
        getSplittedStringOnIndex(filename, buffer, '-', 3);
        rm(filename, parentIdx[0]);
    }else{
        printString("Too many arguments");
    }

    return 0;
}

void rm(char *name, char *currDir) {
    char map[512];
    char files[1024];
    char sector[512];
    int targetFileIdx;
    int idx;
    char status[16];

    readSector(map, 0x100);
    readSector(files, 0x101);
    readSector(files+512, 0x102);
    readSector(sector, 0x103);

    targetFileIdx = getIdxFromFile(name, currDir, files);
    if(targetFileIdx==-1){
        printString("File not found!");
        return;
    }

    if(files[targetFileIdx*16 + 1]==0xff){
        printString("Deleting folder...");
        deleteDirectory(targetFileIdx,map, files, sector);
    }else{
        printString("Deleting File...");
        deleteFile(targetFileIdx, map, files, sector);
    }

    interrupt(0x21, 0x06, "shell", 0x3000, status);
}
