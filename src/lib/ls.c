int main() {
    interrupt(0x21, 0, "Hello this is from ls");
    while(1);
    return 0;
}