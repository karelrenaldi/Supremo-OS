void ln(char* pathReference, char* filename, char currIdx);

int main() {
    char files[1024];
    char buffer[512 * 16];
    char numOfArgs[255], parentIdx[255], sourcefile[255], targetfile[255];
    int flag, idx, status;

    readSector(files, 0x101);
    readSector(files + 512, 0x102);
    
    clear(buffer, 512 * 16);
    readFile(buffer, "rpl/~msg", &flag, 0xFF);
    printString(buffer);

    getSplittedStringOnIndex(numOfArgs, buffer, '-', 0);
    if(numOfArgs[0] == 3){
        getSplittedStringOnIndex(parentIdx, buffer, '-', 1);
        getSplittedStringOnIndex(sourcefile, buffer, '-', 2);
        getSplittedStringOnIndex(targetfile, buffer, '-', 3);
        ln(sourcefile, targetfile, parentIdx[0]);
    }else{
        printString("Too many arguments");
    }

    interrupt(0x21, 0x06, "shell", 0x3000, status);
    return 0;
}

void ln(char* pathReference, char* filename, char currIdx) {
    char files[1024];
    char pathReferenceIdx, filenameIdx;
    char status[16];
    int i, emptyIdx = -1, idx;

    interrupt(0x21, 2, files, 0x101, 0);
    interrupt(0x21, 2, files + 512, 0x102, 0);

    pathReferenceIdx = idxPath(pathReference, files, currIdx);
    filenameIdx = idxPath(filename, files, currIdx);

    if(filenameIdx != 0x40) {
        printString("File sudah ada!");
    }else if(pathReferenceIdx == 0x40){
        printString("File tidak ditemukan");
    }else{
        for(i = 0; i < 64; i++) {
            if(files[(i * 16) + 2] == 0) {
            emptyIdx = i;
            break;
            }
        }

        if(emptyIdx == -1) {
            printString("Entry file sudah penuh");
            return;
        }

        files[emptyIdx * 16] = currIdx;
        files[(emptyIdx * 16) + 1] = files[(pathReferenceIdx * 16) + 1];

        i = 0;
        while (i < 14 && filename[i] != 0){
            files[(emptyIdx * 16) + 2 + i] = filename[i];
            i++;
        }

        interrupt(0x21, 3, files, 0x101, 0);
        interrupt(0x21, 3, files + 512, 0x102, 0);
    }
}