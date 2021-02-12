// -*- coding : utf-8; -*-
// g++ -o main main.cpp -lncursesw

#include <ncurses.h>
#include <curses.h>
#include <locale.h>
#include <signal.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#define ctrl(x) ((x)&0x1f)

// main function
int main(int argc, char *argv[]) {
  if(argc >= 3) {
    fprintf(stderr, "\033[1m\033[31mnow under construction. it will be openend multiple files\033[m\n");
    return 0;
  }
  else if (argc < 2) {
    fprintf(stderr, "\033[1m\033[31mPlease specify the file name\033[m\n");
    return 0;
  }

  std::ifstream ifs(argv[1]);
  std::ofstream ofs;

  // initial setting
  initscr();
  curs_set(1);
  setlocale(LC_ALL, "");
  start_color();
  noecho();
  keypad(stdscr, TRUE);

  
  if(ifs.fail()) {
    // for debug
    fprintf(stdout, "\033[1m\033[31mfile doesn't exist. (this comment is debug mode, so will be removed)\033[m\n");
  } else {
    // display the contents of a file in the terminal.
  }
  
  endwin();
  return 0;
}
