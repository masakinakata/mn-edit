#include "init.hpp"
#include "help.hpp"

#define DEBUG_MODE 1

int width = 0, height = 0;
int current_height = 0, current_width = 0;
int initx = 1, inity = 0;
int endx = 1, endy = 2;
bool cursor_set = false;
bool redo_set = false;
int x_cur = initx, y_cur = inity;
int space = 0;
int x_set = initx, y_set = inity; // for ctrl(' '), starting point of changing area.
char* file_name;
int line_program = 0;
std::vector<std::string> color_first = {"for","while","do","return","if","else", "using", "namespace", "static", "public", "private", "case", "default","class","break","switch"};
std::vector<std::string> color_second = {"int", "short", "double", "float", "long", "char","bool","void","unsigned"};

std::vector<std::vector<Key>> key(1, std::vector<Key>(0));
std::vector<Redo> redo_log;

class Static_Key {
public:
  static void print(int key);
  static void insert(int ch);
    // use when an input is '{' or '}'
  static void set_indent(bool positive) {
    // set indent
    int x = x_cur - initx - 1;
    int y = y_cur - inity;
    int indent = 0;
    if(x > 0) indent = key[y][x-1].get_indent();
    else if(y > 0) indent = key[y-1][key[y-1].size()-1].get_indent();
    if(positive) key[y][x].set_indent(indent+1);
    else key[y][x].set_indent((int) std::max(indent-1,0));
    for(int i = x+1; i < key[y].size(); i++) {
      if(positive) key[y][i].set_indent(key[y][i].get_indent()+1);
      else key[y][i].set_indent((int) std::max(key[y][i].get_indent()-1,0));
    }
    for(int i = y+1; i < key.size(); i++) {
      for(int j = 0; j < key[i].size(); j++) {
	if(positive) key[i][j].set_indent(key[i][j].get_indent()+1);
	else key[i][j].set_indent((int) std::max(key[i][j].get_indent()-1,0));
      }
    }
  }
  static void set_indent_all() {
    int indent = 0;
    for(int i = 0; i < key.size(); i++) {
      for(int j = 0; j < key[i].size(); j++) {
	int key_tmp = key[i][j].get_key();
	if(key_tmp == '{') {
	  indent++;
	  key[i][j].set_indent(indent);
	} else if(key_tmp == '}') {
	  indent--;
	  key[i][j].set_indent(indent);
	} else key[i][j].set_indent(indent);
      }
    }
  }

  static void erase() {
    int x = x_cur - initx - 1, y = y_cur - inity;
    if(x >= 0) {
      redo_log.push_back(Redo(x_cur-1,y_cur,key[y][x].get_key()));
      key[y].erase(key[y].begin() + x);
      movec(-1, 0);
      delch();
      Color::set_color_all();
      set_indent_all();
      redisplay_line();
    } else {
      if(y != inity) {
	if(y == inity + current_height) {
	  scroll_win(-1);
	}
	deleteln();
	x_cur = key[y-1].size() + initx; y_cur--;
	moveh(y_cur, x_cur);
	for(int i = 0; i < key[y].size(); i++) {
	  key[y-1].push_back(key[y][i]);
	  addch(key[y][i].get_key());
	}
	key.erase(key.begin()+y);
	moveh(y_cur, x_cur);
	redo_log.push_back(Redo(x_cur,y_cur,'\n'));
      }
      Color::set_color_all();
      set_indent_all();
      redisplay();
    }
  }
  static void input(int ch);
  static void enter();
  static void enter_redo();
};

bool Variable::atcoder_flag = false;
int Variable::atcoder_number = 100;
std::string Variable::atcoder_name = "";
char Variable::atcoder_task = 'A';

int main(int argc, char* argv[]) {
  // keybind
  system("stty intr undef");  // ctrl+C
  system("stty stop undef");  // ctrl+S
  system("stty start undef"); // ctrl+Q
  
  std::ifstream ifs;
  switch(argc) {
  case 1:
    std::cerr << "Set file" << std::endl;
    return 0;
  case 2:
    file_name = argv[1];
    ifs = std::ifstream(argv[1], std::ios::in);
    if(ifs.fail()) break;
    {
      int i = 0;
      while(!ifs.eof()) {
	if(i != 0) key.resize(i+1);
	std::string line;
	std::getline(ifs, line);
	int length = 0;
	for(int j = 0; j < line.length(); j++) {
	  // x_cur++;
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
	  // x_cur = initx;
	  // y_cur++;
	  length++;
	}
	i++;
      }
      x_cur = initx;
      y_cur = inity;
    }
    Color::set_color_all();
    line_program = key.size();
    break;
  default :
    std::cerr << "Arguments are too large" << std::endl;
    return 0;
  }
  
  initscr();
  start_color();
  noecho();
  setlocale(LC_ALL, "");
  keypad(stdscr, TRUE);
  scrollok(stdscr, TRUE);

  Static_Key::set_indent_all();
  redisplay();
  
  bool True = true;
  int ch;
  
  while(True) {
    ch = getch();
#ifdef DEBUGMODE
    char debug[256];
    sprintf(debug,"%d", ch);
    mode_display(debug);
    moveh(y_cur, x_cur);
#endif
    switch(ch) {
    case ctrl('w'):
      move(0, -1);
    case KEY_UP:
      movec(0, -1);
      break;
    case ctrl('x'):
      movec(0, 1);
    case KEY_DOWN:
      movec(0, 1);
      break;
    case ctrl('d'):
      {
	int x = x_cur-initx, y = y_cur-inity;
	bool loop = true;
	for(int i = x; i < key[y].size() && loop; i++) {
	  switch(key[y][i].get_key()) {
	  case ' ':
	  case ',':
	  case ':':
	  case ';':
	  case '%':
	  case '@':
	    if(i == x) movec(1, 0);
	    loop = false;
	    break;
	  default :
	    movec(1, 0);
	    break;
	  }
	}
      }
      break;
    case KEY_RIGHT:
      movec(1, 0);
      break;
    case ctrl('a'):
      x_cur = initx;
      current_width = 0;
      moveh(y_cur, x_cur);
      break;
    case ctrl('e'):
      x_cur = initx + key[y_cur-inity].size() + 1;
      // current_width = std::max(0, x_cur - width);
      moveh(y_cur, x_cur);
    case KEY_LEFT:
      movec(-1, 0);
      break;
    case ctrl('q'):
      if(cursor_set) {
	cursor_set = false;
	mode_display("Quit :: cursor_set");
      }
      else True = false;
      break;
    case ctrl('s'):
      {
	std::ofstream ofs = std::ofstream(argv[1]);
	for(int i = 0; i < key.size(); i++) {
	  for(int j = 0; j < key[i].size(); j++) {
	    ofs << (char) key[i][j].get_key();
	  } 
	  if(i != key.size()-1) ofs << std::endl;
	}
	
	ofs.close();
	save_display(argv[1]);
      }
      break;
    case ctrl('r'):
      if(redo_log.size() > 0) {
	mode_display("redo");
	Redo log = redo_log.back();
	redo_log.pop_back();
	x_cur = log.x;
	y_cur = log.y;
	moveh(y_cur, x_cur);
	if(log.key == '\n') Static_Key::enter_redo();
	else {
	  Static_Key::insert(log.key);
	  Static_Key::print(log.key);
	}
      } else mode_display("redo empty");
      moveh(y_cur, x_cur);
      break;
    case ctrl('u'):
      mode_display("undo");
      moveh(y_cur, x_cur);
      break;
    case ctrl(' '):
      cursor_set = true;
      x_set = x_cur;
      y_set = y_cur;
      mode_display("cursor set");
      moveh(y_cur, x_cur);
      break;
    case KEY_BACKSPACE:
      redo_set = true;
      cursor_set = false;
      Static_Key::erase();
      break;
    case KEY_Enter:
      if(redo_set) {
	redo_log.clear();
	redo_set = false;
      }
      Static_Key::enter();
      break;
    case ctrl('f'):
      cursor_set = false;
      search_word();
      moveh(y_cur, x_cur);
      break;
    case ctrl('c'):
      mode_display("copy :: under construction");
      moveh(y_cur, x_cur);
      break;
    case ctrl('v'):
      mode_display("paste :: under construction");
      moveh(y_cur, x_cur);
      break;
    case ctrl('h'):
      mode_display("ctrl-h");
      {
	int ch_help;
	ch_help = getch();
	switch(ch_help) {
	case ctrl('a'):
	  {
	    atcoder(argv[1]);
	  }
	  break;
	case ctrl('h'):
	  {
	    help();
	  }
	  break;
	case ctrl('n'):
	  {
	    new_program();
	  }
	  break;
	case ctrl('t'):
	  {
	    terminal();
	  }
	  break;
	default :
	  mode_display("under construction mode");
	  break;
	}
      }
      moveh(y_cur, x_cur);
      break;
    case RESIZE_WINDOW:
      redisplay();
      break;
    case TAB:
      cursor_set = false;
      for(int i = 0; i < 2; i++) {
	Static_Key::insert(' ');
	Static_Key::print(' ');
      }
      break;
    case ALT:
      ch = getch();
      switch(ch) {
      case '<':
	x_cur = initx;
	y_cur = inity;
	current_width = 0;
	current_height = 0;
	redisplay();
	break;
      case '>':
	x_cur = initx + key.back().size();
	y_cur = inity + key.size() - 1;
	if(x_cur > width) current_width = x_cur-width+1;
	if(y_cur > height) current_height = y_cur-height+1;
	redisplay();
	break;
      default:
	break;
      }
      break;
    default:
      cursor_set = false;
      if(redo_set) {
	redo_log.clear();
	redo_set = false;
      }
      Static_Key::input(ch);
      break;
    }
  }
  endwin();
  /*
  for(int i = 0; i < key.size(); i++) {
    for(int j = 0; j < key[i].size(); j++) {
      printf("%s\n",key[i][j].get_word().c_str());
    }
    printf("\n");
  }
  */
  return 0;
}

void Static_Key::print(int key) {
  if(x_cur >= width + current_width-1) {
    Color::set_color_all();
    slide_win(1);
    x_cur++;
    moveh(y_cur, x_cur);
  }
  else {
    insch(key);
    Color::set_color_line();
    redisplay_line();
    movec(1, 0);
  }
}

void Static_Key::insert(int ch) {
    int x = x_cur - initx, y = y_cur - inity;
    if(x >= 0) {
      key[y].insert(key[y].begin() + x, ch);
      if(x != 0 && key[y][x].get_key() != ' ') key[y][x].set_word(key[y][x-1].get_word()+(char)ch);
      if(x > 0) key[y][x].set_indent(key[y][x-1].get_indent());
    }
    Color::set_color();
}

void Static_Key::input(int ch) {
  Static_Key::insert(ch);
  Static_Key::print(ch);
  if(ch == '{') Static_Key::set_indent(true);
  if(ch == '}') {
    int x = x_cur - initx - 2;
    int y = y_cur - inity;
    bool back = true;
    if(x <= 0) back = false;
    for(int i = 0; i < x; i++) {
      if(key[y][x].get_key() != ' ') {
	back = false;
	break;
      }
    }
    
    if(back) {
      moveh(y_cur, initx);
      int size = key[y].size();
      for(int i = 0; i < 2 && i < size; i++) {
	key[y].erase(key[y].begin());
	delch();
	x_cur--;
      }
      moveh(y_cur, x_cur);
    }
    Static_Key::set_indent(false);
  }
}

void Static_Key::enter() {
  cursor_set = false;
  moveh(y_cur+1, x_cur);
  insertln();
  {
    int x = x_cur - initx, y = y_cur - inity;
    std::vector<Key> tmp(0);
    moveh(y_cur, x_cur);
    for(int i = x; i < key[y].size(); i++) {
      delch();
    }
    
    int yy = y;
    while(true) {
      if(yy < 0) {
	space = 0;
	break;
      }
      if(key[yy].size() > 0) {
	space = key[yy][key[yy].size()-1].get_indent();
	break;
      }
      yy--;
    }
    x_cur = initx;
    y_cur++;
    x_cur = initx + space*2;
    for(int i = 0; i < space*2; i++) {
      tmp.push_back(Key(' '));
      tmp[i].set_indent(space);
    }
    moveh(y_cur, x_cur);
    int loop = key[y].size()-x;
    for(int i = 0; i < loop; i++) {
      addch(key[y][x].get_key());
      tmp.push_back(key[y][x]);
      key[y].erase(key[y].begin()+x);
    }
    key.insert(key.begin()+y+1, tmp);
  }
  if(y_cur >= height+current_height) {
    scroll_win(1);
  }
  Color::set_color_all();
  redisplay();
  moveh(y_cur, x_cur);
}


void Static_Key::enter_redo() {
  cursor_set = false;
  moveh(y_cur+1, x_cur);
  insertln();
  {
    int x = x_cur - initx, y = y_cur - inity;
    std::vector<Key> tmp(0);
    moveh(y_cur, x_cur);
    for(int i = x; i < key[y].size(); i++) {
      delch();
    }

    
    x_cur = initx;
    y_cur++;
    x_cur = initx;
    moveh(y_cur, x_cur);
    int loop = key[y].size()-x;
    for(int i = 0; i < loop; i++) {
      addch(key[y][x].get_key());
      tmp.push_back(key[y][x]);
      key[y].erase(key[y].begin()+x);
    }
    key.insert(key.begin()+y+1, tmp);
  }
  if(y_cur >= height+current_height) {
    scroll_win(1);
  }
  Color::set_color_all();
  redisplay();
  moveh(y_cur, x_cur);
}

void getwh() {
  int width_tmp, height_tmp;
  getmaxyx(stdscr, height_tmp, width_tmp);
  width = width_tmp - endx;
  height = height_tmp - endy;
}

void movec(int x_diff, int y_diff) {
  if(x_diff != 0 && y_diff != 0) { // error case
    endwin();
    printf("should debug\n");
    exit(0);
  }
  
  if(x_diff != 0) { // KEY_RIGHT or KEY_LEFT
    if(x_diff == 1) { // KEY_RIGHT
      x_cur++;
      if(x_cur == key[y_cur-inity].size()+1+initx){
	if(y_cur < key.size()-1) {
	  x_cur = initx;
	  y_cur++;
	  if(current_width > 0) {
	    current_width = 0;
	    redisplay();
	  }
	  if(y_cur == height+current_height) scroll_win(1);
	} else x_cur--;
      } else if (x_cur == width+current_width) slide_win(1);
    } else { // KEY_LEFT
      x_cur--;
      if(x_cur < initx) {
	if(y_cur > inity) {
	  x_cur = key[y_cur-inity-1].size() + initx;
	  y_cur--;
	  if(x_cur > width) {
	    current_width = x_cur-width;
	    redisplay();
	  }
	  if(y_cur < inity+current_height) scroll_win(-1);
	} else x_cur++;
      } else if(x_cur < initx+current_width) slide_win(-width);
    }
  } else { // KEY_UP or KEY_DOWN
    // must write for new page
    y_cur += y_diff;
    if(y_cur == key.size()) {
      y_cur--;
    } else if (y_cur < inity) {
      y_cur++;
    } else if (y_cur < inity+current_height) scroll_win(-(height+1)/2);
    else if (y_cur == height+current_height) scroll_win((height+1)/2);
    if(key[y_cur-inity].size() + initx < x_cur) x_cur = key[y_cur-inity].size() + initx; 
  }
  moveh(y_cur, x_cur);
}

void moveh(int y, int x) {
  move(y-current_height, x-current_width);
}
