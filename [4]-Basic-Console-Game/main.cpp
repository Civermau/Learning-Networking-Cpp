#include "nonblocking_input.h"
// #include "ui.h"
#include "Server-Side/server.cpp"
#include "Client-Side/client.cpp"
#include <chrono>
#include <iostream>
#include <thread>

int main() {
  // hideCursor();

#ifndef _WIN32
  // POSIX: enable raw/noncanonical mode once at program start
  enableRawMode();
#endif

  int lastX = -1, lastY = -1;
  Server* server = nullptr;
  Client* client = nullptr;

  for (;;) {
    auto [x, y] = getConsoleSize();

    if (lastX != x || lastY != y) {
      clearScreen();
      lastX = x;
      lastY = y;
    }

    printOptionsList(3, "Host", "Join", "Quit");

    int ch = getCharNonBlocking();
    if (ch != -1) {
      if (ch == '1'){
        delete server;
        server = new Server(1234);
        server->start();
        break;
      }

      if (ch == '2'){
        delete client;
        client = new Client(1234);
        break;
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

  delete client;
  delete server;
  return 0;
}
