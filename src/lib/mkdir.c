void mkdir(char* filename, int* flag, char parentIdx);

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
        getSplittedStringOnIndex(filename, buffer, '-', 2);
        mkdir(filename, &flag, parentIdx[0]);
    }else{
        printString("Too many arguments");
    }

    interrupt(0x21, 0x06, "shell", 0x3000, status);

    return 0;
}

void mkdir(char* filename, int* flag, char parentIdx){
    char files[1024], filenameIdx;
    int i, fileSectorIdx = -1;

    *flag = 0;

    readSector(files, 0x101);
    readSector(files + 512, 0x102);

    for(i = 0; i < 64; i++) {
        if(files[(i * 16) + 2] == 0) {
        fileSectorIdx = i;
        break;
        }
    }

    if(fileSectorIdx == -1) {
        *flag = 1;
        return;
    }

    filenameIdx = idxPath(filename, files, parentIdx);
    if(filenameIdx != 0x40) {
        *flag = 2;
        return;
    }else{
        files[fileSectorIdx * 16] = parentIdx;
        files[(fileSectorIdx * 16) + 1] = 0xFF;

        i = 0;
        while (i < 14 && filename[i] != 0){
        files[(fileSectorIdx * 16) + 2 + i] = filename[i];
        i++;
        }

        interrupt(0x21, 3, files, 0x101, 0);
        interrupt(0x21, 3, files + 512, 0x102, 0);
    }
}