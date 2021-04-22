int main() {
    char files[1024], status[16];
    int i, j;

    interrupt(0x21, 2, files, 0x101, 0);
    interrupt(0x21, 2, files + 512, 0x102, 0);

    for (i = 0; i < 64; i++){
        j = 0;
        
        // Current index ganti 0xFF
        if(files[i*16] == 0xFF){
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

    interrupt(0x21, 0x06, "shell", 0x3000, status);
    return 0;
}