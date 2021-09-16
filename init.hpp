#ifndef INIT_HPP
#define INIT_HPP

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>


#define ctrl(x) ((x) & 0x1f)
#define TAB 9
#define KEY_Enter 10
#define ALT 27
#define RESIZE_WINDOW 410

#define COLOR_ORANGE 8
#define COLOR_PURPLE 9

extern int width, height;
extern int current_height, current_width;
extern int initx, inity;
extern int endx, endy;
extern int x_cur, y_cur;
extern int space;
extern char* file_name;
extern int line_program;
extern std::vector<std::string> color_first;
extern std::vector<std::string> color_second;

void init_display();
void save_display(char *filename);
void mode_display(const char *change_mode);
void redisplay_base();
void redisplay_line();
void redisplay();
void scroll_win(int n);
void slide_win(int n);

class Key {
  int key;
  int color;
  int space;
  std::string word;
  int indent;
public:
  Key() : key(0), color(0), word(""), indent(0) {}
  Key(int key) : key(key), color(0), word(""), indent(0) {}
  Key(int key, int color) : key(key), color(color), word(""), indent(0) {}
  int get_key() { return key;}
  void set_key() { this->key = key;}
  int get_color() { return color;}
  void set_color(int color) { this->color = color;}
  std::string get_word() { return word;}
  void set_word(std::string word) { this->word = word;}
  int get_indent() { return indent;}
  void set_indent(int indent) { this->indent = indent;}
};

extern std::vector<std::vector<Key>> key;

class Variable {
public:
  static bool atcoder_flag;
  static int atcoder_number;
  static std::string atcoder_name;
  static char atcoder_task;
};

void getwh();
void movec(int x_diff, int y_diff);
void moveh(int y, int x);
void search_word();

class Color {
public:
  static void set_color() {
    int x = x_cur - initx, y = y_cur - inity;
    char key_tmp = key[y][x].get_key();
    if(x > 0 && key[y][x-1].get_color() == 7) {
      key[y][x].set_color(7);
      return;
    }
    
    if(key_tmp == '"') {
      if(x > 0 && key[y][x-1].get_color() == 3) {
	if(key[y][x-1].get_key() == '\\') {
	  if(x > 1 && key[y][x-2].get_key() == '\\') key[y][x].set_color(4);
	  else key[y][x].set_color(3);
	} else key[y][x].set_color(4);
      }       
      else key[y][x].set_color(3);
      return;
    }
    if(key_tmp == '\'') {
      if(x > 0 && key[y][x-1].get_color() == 8) {
	if(key[y][x-1].get_key() == '\\') {
	  if(x > 1 && key[y][x-2].get_key() == '\\') key[y][x].set_color(9);
	  else key[y][x].set_color(8);
	} else key[y][x].set_color(9);
      }
      else key[y][x].set_color(8);
      return;
    }
    if(x > 0 && (key[y][x-1].get_color() == 3 || key[y][x-1].get_color() == 8)) {
      key[y][x].set_color(key[y][x-1].get_color());
      return;
    }
    if((key_tmp < 48 || key_tmp > 57) && (key_tmp < 65 || key_tmp > 90) && (key_tmp < 97 || key_tmp > 122) && key_tmp != ':' && key_tmp != '_') {
      switch(key_tmp) {
      case '>':
      case '<':
      case '=':
      case '!':
      case ';':
	key[y][x].set_color(6);
	break;
      case '/':
	if(x > 0 && key[y][x-1].get_key() == '/') {
	  key[y][x-1].set_color(7);
	  key[y][x].set_color(7);
	}
	if(x>0 && key[y][x].get_color() == 8 && key[y][x-1].get_key() == '*') {
	  key[y][x].set_color(9);
	}
	break;
      case '*':
	if(x>0 && key[y][x].get_key() == '/') {
	  key[y][x-1].set_color(8);
	  key[y][x].set_color(8);
	}
	break;
      default:
	key[y][x].set_color(0);
	break;
      }
      return;
    }
    
    std::string reset_word = "";
    for(int i = x; i >= 0; i--) {
      char key_reset = key[y][i].get_key();
      if((key_reset < 48 || key_reset > 57) && (key_reset < 65 || key_reset > 90) && (key_reset < 97 || key_reset > 122) && key_reset != '#' && key_reset != '_'){
	break;
      }
      reset_word = ((char)key[y][i].get_key()) + reset_word;
    }
    key[y][x].set_word(reset_word);
    
    int length = key[y][x].get_word().length();
    for(int i = 0; i < length; i++) {
      key[y][x-i].set_color(0);
    }
    
    for(int i = 0; i < color_first.size(); i++) {
      if(color_first[i] == key[y][x].get_word()) {
	for(int i = 0; i < length; i++) {
	  key[y][x-i].set_color(1);
	}
	return;
      }
    }
    
    for(int i = 0; i < color_second.size(); i++) {
      if(color_second[i] == key[y][x].get_word()) {
	for(int i = 0; i < length; i++) {
	  key[y][x-i].set_color(2);
	}
	return;
      }
    }
    
    if(key[y].size() > 2 && key[y][x].get_key() == ':' && key[y][x-1].get_key() == ':') {
      if(key[y][x-2].get_color() != 0) return;
      int length_tmp = key[y][x-2].get_word().length();
      for(int i = 0; i < length_tmp; i++) {
	key[y][x-2-i].set_color(5);
      }
    }
    
    if(reset_word[0] == '#') {
      for(int i = 0; i < length; i++) {
	key[y][x-i].set_color(1);
      }
    }
  }
  static void set_color_line() {
    int x_tmp = x_cur;
    for(int j = 0; j < key[y_cur-inity].size(); j++) {
      x_cur = initx + j;
      set_color();
    }
    x_cur = x_tmp;
  }
  static void set_color_all() {
    int x_tmp = x_cur, y_tmp = y_cur;
    for(int i = 0; i < key.size(); i++) {
      y_cur = inity+i;
      for(int j = 0; j < key[i].size(); j++) {
	x_cur = initx+j;
	set_color();
      }
    }
    x_cur = x_tmp;
    y_cur = y_tmp;
  }
};

class Redo {
public:
  int x,y,key;
  Redo(int x, int y, int key) : x(x), y(y), key(key) {}
};
extern std::vector<Redo> redo_log;
#endif
