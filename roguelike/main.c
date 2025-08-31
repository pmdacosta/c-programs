#include <ncurses.h>

void screenSetup() {
    initscr();
    printw("Hello World");
    noecho();
    refresh();
}

int main() {
    
    return 0;
}
