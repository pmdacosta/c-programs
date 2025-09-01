#include <ncurses.h>

int main() {
    char text[] = "Greetings from Ncurses!";
    char *t;

    initscr();
    t = text;       // initiliaze the pointer

    while(*t) {
        addch(*t);  // put on char in the screen
        t++;        // move pointer to next position
        refresh();  // refresh the screen (render)
        napms(100); // delay .1 sec
    }
    getch();        // wait for user input

    endwin();       // clean up ncurses
    return 0;
}
