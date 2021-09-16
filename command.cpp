#include "init.hpp"

void search_word() {
  mode_display("search word");
  int ch_find;
  std::string word = "";
  while(true) {
    ch_find = getch();
    if(ch_find == KEY_Enter) break;
    if(ch_find == KEY_BACKSPACE) {
      word = word.substr(0,word.length()-1);
      mode_display("search word");
      printw("%s",word.c_str());
    }
    if(65 <= ch_find && ch_find <= 122) {
      addch(ch_find);
      word += (char) ch_find;
    }
  }
  
  bool not_found = true;
  for(int i = 0; i < key.size() && not_found; i++) {
    for(int j = 0; j < key[i].size(); j++) {
      if(word == key[i][j].get_word()) {
	not_found = false;
	break;
      }
    }
  }
  if(not_found) mode_display("Not Found");
  else mode_display("Found");
}
