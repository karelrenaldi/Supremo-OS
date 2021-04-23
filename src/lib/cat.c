void cat(char* pathReference, char currIdx);

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
        cat(filename, parentIdx[0]);
    }else{
        printString("Too many arguments");
    }

    return 0;
}

void cat(char* pathReference, char currIdx) {
    int status;
    char files[1024];
    char fileBuffer[20 * 512];
    char fileIdx;
    int idx;
    char status2[16];

    interrupt(0x21, 2, files, 0x101, 0);
    interrupt(0x21, 2, files + 512, 0x102, 0);
    fileIdx = idxPath(pathReference, files, currIdx);

    if(*pathReference) {
        interrupt(0x21, 0x4 | (currIdx) << 8, fileBuffer, pathReference, &status);
        if (status == 0) {
            interrupt(0x21, 0x0, fileBuffer, 0x0, 0x0);
        }
        else {
            interrupt(0x21, 0x0, "Tidak dapat membaca file.", 0x0, 0x0);
        }
    }
    else {
        interrupt(0x21, 0x0, "Tidak dapat membaca file.", 0x0, 0x0);
    }

    interrupt(0x21, 0x06, "shell", 0x3000, status2);
}