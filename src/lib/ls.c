void ls(unsigned char parentIndex);

int main() {
    char files[1024];
    char buffer[512 * 16];
    char args1[255];
    char args2[255];
    int flag, idx, status;

    readSector(files, 0x101);
    readSector(files + 512, 0x102);
    
    clear(buffer, 512 * 16);
    readFile(buffer, "rpl/~msg", &flag, 0xFF);

    getSplittedStringOnIndex(args1, buffer, '-', 0);
    if(args1[0] == 1){
        getSplittedStringOnIndex(args2, buffer, '-', 1);
        ls(args2[0]);
    }else{
        printString("To many arguments");
    }

    interrupt(0x21, 0x06, "shell", 0x3000, status);

    return 0;
}

void ls(unsigned char parentIndex) {
    char files[1024], status[16];
    int i, j, idx;

    interrupt(0x21, 2, files, 0x101, 0);
    interrupt(0x21, 2, files + 512, 0x102, 0);

    for (i = 0; i < 64; i++){
        j = 0;
        if(files[i*16] == parentIndex){
            if(files[i*16 + 1] == 0xFF){
                while(files[i*16 + 2 + j] != 0){
                    interrupt(0x10, (0x0e << 8) + files[i*16 + 2 + j], 0x0, 0x0, 0x0);
                    j++;
                } 
                interrupt(0x10, (0x0e << 8) + '/', 0x0, 0x0, 0x0);
            } else {
                while(files[i*16 + 2 + j] != 0){
                    interrupt(0x10, (0x0e << 8) + files[i*16 + 2 + j], 0x0, 0x0, 0x0);
                    j++;
                } 
            }
            
            interrupt(0x10, (0x0e << 8) + '\n', 0x0, 0x0, 0x0);
            interrupt(0x10, (0x0e << 8) + '\r', 0x0, 0x0, 0x0);
        }
    }
}