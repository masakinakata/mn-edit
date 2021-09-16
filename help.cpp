#include "init.hpp"
#include "help.hpp"
#include <string.h>
#include <sstream>

void atcoder(char* argv) {
  int ch_help;
  std::string submit_test;
  mode_display("ctrl-h + ctrl-a(atcoder) :: submit('s') or test('t')");
  ch_help = getch();
  if(ch_help == 's') {
    submit_test = "submit";
  } else if (ch_help == 't') {
    submit_test = "test";
  }  else {
    mode_display("quit atcoder-mode");
    return;
  }
  if(!Variable::atcoder_flag){
    mode_display("input atcoder competition");
    std::string atcoder_word = "";
    for(int i = 0; i < 3; i++) {
      ch_help = getch();
      if(ch_help == ctrl('q')) {
	mode_display("quit atcoder-mode");
	moveh(y_cur, x_cur);
	return;
      }
      if(ch_help == KEY_BACKSPACE) {
	int x, y;
	getyx(stdscr,y,x);
	move(y, x-1);
	delch();
	i -= 2;
      } else {
	addch(ch_help);
	atcoder_word += (char)ch_help;
      }
    }
    transform (atcoder_word.begin(), atcoder_word.end(), atcoder_word.begin(), tolower);
    if(atcoder_word == "abc") {
      mode_display("submit ABC :: input number");
      Variable::atcoder_name = "ABC";
      Variable::atcoder_flag = true;
    } else if(atcoder_word == "arc") {
      mode_display("submit ARC :: input number");
      Variable::atcoder_name = "ARC";
      Variable::atcoder_flag = true;
    } else if(atcoder_word == "agc") {
      mode_display("submit AGC :: input number");
      Variable::atcoder_name = "AGC";
      Variable::atcoder_flag = true;
    } else {
      mode_display("quit atcoder-mode :: invalid name");
      return;
    }
    Variable::atcoder_number = 100;
    std::string str_num = "";
    while(true) {
      ch_help = getch();
      addch(ch_help);
      if(ch_help == ctrl('q')) {
	mode_display("quit atcoder-mode");
	Variable::atcoder_flag = false;
	return;
      }
      if(ch_help == KEY_Enter) {
	bool valid = true;
	for(int i = 0; i < str_num.length(); i++) {
	  if('0' > str_num[i] || '9' < str_num[i]) {
	    mode_display("quit atcoder-mode :: invalid number");
	    str_num = "";
	    valid = false;
	    break;
	  }
	}
	if(valid) {
	  Variable::atcoder_number = std::stoi(str_num);
	  break;
	}
      } else str_num += (char) ch_help;
    }
  }
  if(Variable::atcoder_flag) {
    char display_line[20];
    sprintf(display_line, "%s%d :: %s :: input task",Variable::atcoder_name.c_str(), Variable::atcoder_number,submit_test.c_str());
    mode_display(display_line);
    while(true) {
      int task = getch();
      if(task == KEY_Enter) break;
      addch(task);
      Variable::atcoder_task = task;
    }
    if(submit_test == "submit") {
      char submit_line[100];
      sprintf(submit_line,"oj s -y https://atcoder.jp/contests/%s%d/tasks/%s%d_%c %s",Variable::atcoder_name.c_str(),Variable::atcoder_number,Variable::atcoder_name.c_str(),Variable::atcoder_number,Variable::atcoder_task,argv);
      endwin();
      system(submit_line);
      initscr();
      redisplay();
      mode_display(submit_line);
    } else {
      char compile_line[100];
      sprintf(compile_line,"g++ %s", argv);
      system(compile_line);
      char test_line[100];
      system("rm -r test");
      sprintf(test_line,"oj d https://atcoder.jp/contests/%s%d/tasks/%s%d_%c",Variable::atcoder_name.c_str(),Variable::atcoder_number,Variable::atcoder_name.c_str(),Variable::atcoder_number,Variable::atcoder_task);
      endwin();
      system(test_line);
      system("oj t");
      initscr();
      redisplay();
      mode_display(compile_line);
    }
  }
}

void help() {
  clear();
  move(0,0);
  printw("manual command page");
  std::vector<std::string> help = {"ctrl-h : help mode", "   + ctrl-a : atcoder mode", "   + ctrl-h : help mode", "   + ctrl-t : terminal mode", "ctrl-q : quit", "ctrl-f : find word"};
  for(int i = 1; i <= help.size() && i < height; i++) {
    move(i, 0);
    printw("%s",help[i-1].c_str());
  }
  while(true) {
    int ch = getch();
    if(ch == ctrl('q')) break;
  }
  redisplay();
  moveh(y_cur, x_cur);
}

void clear_line(int y, int x_init) {
  move(y,x_init);
  for(int i = x_init; i < width; i++) {
    addch(' ');
  }
  move(y,x_init);
}

void new_program() {
  mode_display("new program");
  std::ifstream ifs = std::ifstream("/home/masaki/work/atcoder_main.cpp", std::ios::in);
  if(ifs.fail()) return;
  key.clear();
  key.resize(1);
  int i = 0;
  while(!ifs.eof()) {
    if(i != 0) key.resize(i+1);
    std::string line;
    std::getline(ifs, line);
    int length = 0;
    for(int j = 0; j < line.length(); j++) {
      if(line[j] == TAB) {
	key[i].push_back(Key(' '));
	key[i].push_back(Key(' '));
	length++;
      } else {
	key[i].push_back(Key(line[j]));
	if(line[j] != ' ') {
	  if(j == 0) {
	    std::string addstr = "";
	    addstr+=line[j];
	    key[i][length].set_word(addstr);
	  }
	  else key[i][length].set_word(key[i][length-1].get_word()+line[j]);
	}
      }
      length++;
    }
    i++;
  }
  x_cur = initx+2;
  y_cur = inity+4;

  Color::set_color_all();
  line_program = key.size();
  redisplay();
}

void terminal() {
  std::vector<std::string> exec_list(0);
  std::string input = "";
  int length = 0;
  int ch;
  clear();
  mode_display("terminal-mode");
  char tmp[256];
  getcwd(tmp, 256);
  int x_init = strlen(tmp)+1;
  int x = x_init, y = 0;
  int latest = -1;
  move(0, 0);
  attron(COLOR_PAIR(10));
  printw("%s$", tmp);
  attroff(COLOR_PAIR(10));
  while(true) {
    ch = getch();
    if(ch == ctrl('q')) break;

    switch(ch) {
    case KEY_BACKSPACE:
      if(x > x_init) {
	x--;
	move(y, x);
	delch();
	input.erase(input.begin()+x-x_init);
      }
      break;
    case KEY_Enter:
      // execution
      if(input != "" ){
	exec_list.push_back(input);
	//	std::string exec_system = input + " > text.txt";
	//system(exec_system.c_str());
	length++;
      }
      //system(input.c_str());
      mode_display(input.c_str());
      y++;
      move(y,0);
      attron(COLOR_PAIR(10));
      getcwd(tmp, 256);
      printw("%s$", tmp);
      attroff(COLOR_PAIR(10));
      x = x_init;
      input = "";
      latest = length-1;
      break;
    case KEY_LEFT:
      if(x > x_init) {
	x--;
	move(y,x);
      }
      break;
    case KEY_RIGHT:
      if(input.length()+x_init > x) {
	x++;
	move(y,x);
      }
      break;
    case KEY_UP:
      if(latest >= 0) {
	if(length == latest) latest--;
	clear_line(y, x_init);
	input = exec_list[latest];
	move(y,x_init);
	printw("%s",exec_list[latest].c_str());
	x = x_init+exec_list[latest].length();
	if(latest != 0) latest--;
	move(y,x);
      }
      break;
    case KEY_DOWN:
      if(latest == -1) latest++;
      if(latest < length) {
	clear_line(y, x_init);
	move(y,x_init);
	input = exec_list[latest];
	printw("%s",exec_list[latest].c_str());
	x = x_init+exec_list[latest].length();
	latest++;
	move(y,x);
      } else if(latest == length) {
	clear_line(y, x_init);
	input = "";
	x = x_init;
	move(y,x);
      } else printw("%d", latest);
      break;
    default:
      if((33 <= ch && ch <= 126) || ch == ' ') {
	addch(ch);
	input += (char) ch;
	x++;
      }
      break;
    } 
  }
  redisplay();
}
