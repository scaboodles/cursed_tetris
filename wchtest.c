#define _XOPEN_SOURCE_EXTENDED
#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

int main() {
    setlocale(LC_ALL, ""); // w i d e  characters
    initscr(); cbreak(); noecho(); // curses bs 
    curs_set(0); // hide cursor
    timeout(-1); // non-blocking getch
    keypad(stdscr, TRUE); // enable arrow keys
    start_color();

    init_pair(1, COLOR_MAGENTA, COLOR_RED);
    init_pair(2, COLOR_GREEN, COLOR_YELLOW);

    wchar_t wch = L'\u2584';

    attron(COLOR_PAIR(1));
    mvwaddwstr(stdscr, 0, 0, &wch);

    refresh();
    getch();

    attron(COLOR_PAIR(2));
    mvwaddwstr(stdscr, 0, 1, &wch);

    refresh();
    getch();
    endwin();
    return 0;
}