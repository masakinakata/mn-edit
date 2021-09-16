#include <stdio.h>
#include <iostream>

int main () {
  int h, w;
  std::cin >> h >> w;
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      std::cout << "Hello World!" << std::endl;
    }
  }
}