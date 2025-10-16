#include "nonblocking_input.h"
// #include "ui.h"
#include "Server-Side/server.cpp"
#include "Client-Side/client.cpp"
#include <chrono>
#include <iostream>
#include <thread>

int askUserForPort();
std::string askUserForIp();

int main() {
  // hideCursor();

#ifndef _WIN32
  // POSIX: enable raw/noncanonical mode once at program start
  enableRawMode();
#endif

  int lastX = -1, lastY = -1;

  // We store `Server` and `Client` as pointers initialized to nullptr because:
  // - We only need an instance conditionally at runtime (based on the menu).
  // - The classes delete copy/assignment (see `server.cpp` and `client.cpp`),
  //   so keeping/by-value reassignment isn't possible; a single pointer "handle"
  //   avoids accidental copies and allows dynamic lifetime control.
  // - Starting as nullptr signals "no active role yet" and lets us (re)create
  //   instances on demand during the loop.
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
        // If an old server exists, delete it first to:
        // - avoid memory leaks and free its bound socket/resources before creating a new one,
        // - ensure only one active `Server` at a time.
        // We call through a pointer (`server->start()`) because we manage it dynamically;
        // by-value usage is disallowed (copy/assign deleted), so pointers/references are required.
        delete server;
        int port = askUserForPort();
        server = new Server(port);
        server->start();
        break;
      }

      if (ch == '2'){
        // Same rationale as above: clean up any previous client before creating a new one.
        // Dynamic allocation lets us opt-in to a client at runtime and respect the
        // non-copyable semantics (copy/assign deleted) of `Client`.
        delete client;
        int port = askUserForPort();
        std::string IP = askUserForIp();
        client = new Client(port, IP);
        break;
      }

      if (ch == 'q' || ch == 'Q') {
        clearScreen();
        std::cout << "Quit pressed\n";
        break;
      }
    }

    // small sleep to avoid burning 100% CPU in the loop
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
  }

  // Final cleanup: delete any created instances to release sockets and memory.
  delete client;
  delete server;
  return 0;
}

int askUserForPort(){
  int port;
  std::string input;
  while (true) {
    std::cout << "Enter a port number (1024–65535): ";
#ifndef _WIN32
    // Switch to canonical mode for blocking line input
    disableRawMode();
    drainStdinBuffer();
#endif
    std::getline(std::cin, input);
#ifndef _WIN32
    // Restore raw mode for non-blocking UI
    enableRawMode();
#endif

    try {
      port = std::stoi(input);
      if (port >= 1024 && port <= 65535)
        break;
      else
        std::cout << "Port must be between 1024 and 65535.\n";
    } catch (const std::exception &) {
      std::cout << "Invalid input. Please enter a valid integer port.\n";
    }
  }
  return port;
}

std::string askUserForIp() {
  std::string input;
  while (true) {
    std::cout << "Enter a server IP address (default 127.0.0.1): ";
#ifndef _WIN32
    disableRawMode();
    drainStdinBuffer();
#endif
    std::getline(std::cin, input);
#ifndef _WIN32
    enableRawMode();
#endif
    if (input.empty())
      input = "127.0.0.1"; 

    // Prepare mbuf to hold returned binary
    unsigned char buf[sizeof(struct in_addr)];
    // Try to parse with inet_pton
    int result = 0;
    // Only test inet_pton if length somewhat plausible
    if (input.size() >= 7 && input.size() <= 45) { // minimal IPv4 or IPv6
      result = inet_pton(AF_INET, input.c_str(), buf);
      if (result == 1) {
        return input;
      }
      result = inet_pton(AF_INET6, input.c_str(), buf);
      if (result == 1) {
        return input;
      }
    }
    std::cout << "Invalid IP address, please enter a valid IPv4 or IPv6 address.\n";
  }
}
