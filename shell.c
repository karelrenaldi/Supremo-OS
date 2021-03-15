#include "shell.h";

void runShell()
{
    interrupt(0x21, 0, "Hello From Shell", 0, 0);
}