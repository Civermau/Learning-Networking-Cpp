#include "ui.h"
#include "nonblocking_input.h"
#include <thread>
#include <chrono>
#include <iostream>

int main() {
  hideCursor();

#ifndef _WIN32
  // POSIX: enable raw/noncanonical mode once at program start
  enableRawMode();
#endif

  int lastX = -1, lastY = -1;
  for (;;) {
    auto [x, y] = getConsoleSize();

    if (lastX != x || lastY != y){
        clearScreen();
        lastX = x;
        lastY = y; 
    }

    printOptionsList(3, "Host", "Join", "Quit");

    int ch = getCharNonBlocking();
    if (ch != -1) {
      if (ch >= '1' && ch <= '3') {
          int choice = ch - '0';
          clearScreen();
          std::cout << "You chose option " << choice << "\n";
          break; // or handle selection as needed
      }
      // optional: handle 'q' to quit quickly
      if (ch == 'q' || ch == 'Q') {
          clearScreen();
          std::cout << "Quit pressed\n";
          break;
      }
    }

    // small sleep to avoid burning 100% CPU in the loop
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
  }

  return 0;
}
