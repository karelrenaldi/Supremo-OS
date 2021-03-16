#define MAX_FILENAME 15
#define MAX_FILES 32
#define SECTOR_SIZE 512
#define DIRS_SECTOR 257
#define FILES_SECTOR 258
#define MAX_DIRS 32
#define ENTRY_LENGTH 16

void main() {
    char currentDir;
    char directories[SECTOR_SIZE];
    char files[SECTOR_SIZE];
    int* result;
    int i, j;
    	
    interrupt(0x21, 0x21, &currentDir, 0, 0);
    interrupt(0x21, 0x02, directories, DIRS_SECTOR);
    interrupt(0x21, 0x02, files, FILES_SECTOR);
    interrupt(0x21, 0x0, "Directories:\r\n", 0, 0);

    char buff[MAX_FILENAME + 1];

    for (i = 0; i < MAX_DIRS; i++) {
        if ((directories[i * ENTRY_LENGTH] == currentDir) && (directories[i * ENTRY_LENGTH + 1] != '\0')) {
            j = 0;
            while ((directories[i * ENTRY_LENGTH + 1 + j] != '\0') && (j < MAX_FILENAME)) {
                buff[j] = directories[i * ENTRY_LENGTH + 1 + j];
                j++;
            }
            buff[j] = '\0';
            interrupt(0x21, 0x0, " - ", 0, 0);
            interrupt(0x21, 0x0, buff, 0, 0);
            interrupt(0x21, 0x0, "\r\n", 0, 0);
        }
    }
    interrupt(0x21, 0x0, "Files:\r\n", 0, 0);
    for (i = 0; i < MAX_FILES; i++) {
        if ((files[i * ENTRY_LENGTH] == currentDir) && (files[i * ENTRY_LENGTH + 1] != '\0')) {
            j = 0;
            while ((files[i * ENTRY_LENGTH + 1 + j] != '\0') && (j < MAX_FILENAME)) {
                buff[j] = files[i * ENTRY_LENGTH + 1 + j];
                j++;
            }
            buff[j] = '\0';
            interrupt(0x21, 0x0, " - ", 0, 0);
            interrupt(0x21, 0x0, buff, 0, 0);
            interrupt(0x21, 0x0, "\r\n", 0, 0);
        }
    }
    interrupt(0x21, 0x07, result, 0, 0);
}