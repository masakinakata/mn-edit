#include "init.hpp"

void init_display() {
  init_color(COLOR_CYAN, 100, 700, 900);
  init_color(COLOR_MAGENTA, 900, 350, 900);
  init_color(COLOR_ORANGE, 900, 600, 200);
  init_color(COLOR_YELLOW, 900,850,200);
  init_color(COLOR_PURPLE, 600,400,900);
  clear();
  getwh();
  init_pair(1, COLOR_BLACK, COLOR_WHITE);
  init_pair(10, COLOR_WHITE, COLOR_CYAN);
  attron(COLOR_PAIR(1));
  move(height+1, 0);
  for(int i = 0; i < width; i++) addch(' ');
  move(height+1, 0);
  printw("file_name : %s", file_name);
  attroff(COLOR_PAIR(1));
  
  //  int maxline = std::min(height-inity, (int)key.size()-current_height);
  int maxline = std::min(height-inity+current_height, (int) key.size());
  int line_tmp = maxline;
  int digit = 0;
  do {
    maxline /= 10;
    digit++;
  } while(maxline != 0);
  move(0, 0);
  attron(COLOR_PAIR(10));
  attron(A_DIM);
  for(int i = current_height; i < line_tmp; i++) {
    for(int j = 0; j < digit; j++) printw(" ");
    move(i-current_height,0);
  }
  move(0, 0);
  for(int i = current_height; i <= line_tmp; i++) {
    printw("%d", i);
    move(i-current_height, 0);
  }
  attroff(COLOR_PAIR(10));
  attroff(A_DIM);
  x_cur -= initx;
  initx = digit;
  x_cur += initx;
  move(inity, initx);
}

void save_display(char *filename) {
  init_pair(1, COLOR_BLACK, COLOR_WHITE);
  attron(COLOR_PAIR(1));
  move(height+1, 0);
  for(int i = 0; i < width; i++) addch(' ');
  move(height+1, 0);
  printw("save file : %s",filename);
  attroff(COLOR_PAIR(1));
  moveh(y_cur, x_cur);
}

void mode_display(const char *change_mode) {
  init_pair(1, COLOR_BLACK, COLOR_WHITE);
  attron(COLOR_PAIR(1));
  move(height+1, 0);
  for(int i = 0; i < width; i++) addch(' ');
  move(height+1, 0);
  printw("%s : ",change_mode);
  attroff(COLOR_PAIR(1));
}

void redisplay_base() {
  moveh(y_cur, initx+current_width);
  int y = y_cur-inity;
  for(int j = current_width; j < width+current_width-initx && j < key[y].size(); j++) {
    int color = key[y][j].get_color();
    if(color == 1) {
      attron(COLOR_PAIR(2));
    } else if (color == 2) {
      attron(COLOR_PAIR(3));
    } else if (color == 3 || color == 4  || color == 8 || color == 9) {
      attron(COLOR_PAIR(4));
    } else if (color == 5) {
      attron(A_BOLD);
      attron(COLOR_PAIR(5));
    } else if (color == 6) {
      attron(COLOR_PAIR(6));
    } else if(color == 7) {
      attron(COLOR_PAIR(7));
    }
    
    addch(key[y][j].get_key());
    attroff(COLOR_PAIR(2));
    attroff(COLOR_PAIR(3));
    attroff(COLOR_PAIR(4));
    attroff(COLOR_PAIR(5));
    attroff(COLOR_PAIR(6));
    attroff(COLOR_PAIR(7));
    attroff(A_BOLD);
  }
}

void redisplay_line() {
  init_pair(2, COLOR_MAGENTA, start_color());
  init_pair(3, COLOR_CYAN, start_color());
  init_pair(4, COLOR_ORANGE, start_color());
  init_pair(5, COLOR_YELLOW, start_color());
  init_pair(6, COLOR_PURPLE, start_color());
  init_pair(7, COLOR_GREEN, start_color());
  redisplay_base();
  moveh(y_cur, x_cur);
}

void redisplay() {
  init_display();
  init_pair(2, COLOR_MAGENTA, start_color());
  init_pair(3, COLOR_CYAN, start_color());
  init_pair(4, COLOR_ORANGE, start_color());
  init_pair(5, COLOR_YELLOW, start_color());
  init_pair(6, COLOR_PURPLE, start_color());
  init_pair(7, COLOR_GREEN, start_color());
  
  move(inity, initx);
  for(int i = current_height; i < height+current_height && i < key.size(); i++) {
    for(int j = current_width; j < width+current_width-initx && j < key[i].size(); j++) {
      int color = key[i][j].get_color();
      if(color == 1) { //first word
	attron(COLOR_PAIR(2));
      } else if (color == 2) { // second word
	attron(COLOR_PAIR(3));
      } else if (color == 3 || color == 4 || color == 8 || color == 9) { // string
	attron(COLOR_PAIR(4));
      } else if(color == 5) { // ~~:: 
	attron(A_BOLD);
	attron(COLOR_PAIR(5));
      } else if(color == 6) { // <>;
	attron(COLOR_PAIR(6));
      } else if(color == 7) { // comment
	attron(COLOR_PAIR(7));
      }
      addch(key[i][j].get_key());
      attroff(COLOR_PAIR(2));
      attroff(COLOR_PAIR(3));
      attroff(COLOR_PAIR(4));
      attroff(COLOR_PAIR(5));
      attroff(COLOR_PAIR(6));
      attroff(COLOR_PAIR(7));
      attroff(A_BOLD);
    }
    move(inity+i+1-current_height, initx);
  }
  //  x_cur = initx; y_cur = inity+current_height;
  moveh(y_cur, x_cur);
}

void scroll_win(int n) {
  current_height += n;
  if(current_height < 0) current_height = 0;
  redisplay();
}

void slide_win(int n) {
  current_width += n;
  if(current_width < 0) current_width = 0;
  redisplay();
}
