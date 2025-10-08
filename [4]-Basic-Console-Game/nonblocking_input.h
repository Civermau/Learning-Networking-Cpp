// I have no idea what this does, I stole it directly from chatgpt
#pragma once

#ifdef _WIN32
  #include <conio.h>   // _kbhit(), _getch()
  // Returns -1 if no key, otherwise the character code (ASCII or extended code).
  inline int getCharNonBlocking() {
    if (_kbhit()) {
      int ch = _getch();
      // Note: _getch() returns 0 or 224 for extended keys, and then the next _getch() is the code.
      // We return the raw values here; caller can interpret them if needed.
      return ch;
    }
    return -1;
  }
#else
  #include <termios.h>
  #include <unistd.h>
  #include <sys/select.h>
  #include <cstdlib>
  #include <cstdio>

  // We keep the original termios so we can restore it on exit.
  static struct termios __orig_termios;
  static bool __raw_mode_enabled = false;

  // Call once at program start to enable raw/noncanonical mode.
  inline void enableRawMode() {
    if (__raw_mode_enabled) return;
    if (tcgetattr(STDIN_FILENO, &__orig_termios) == -1) return;

    struct termios raw = __orig_termios;
    // Turn off canonical mode and echo:
    raw.c_lflag &= ~(ECHO | ICANON);
    // Disable software flow control and carriage return to newline mapping:
    raw.c_iflag &= ~(IXON | ICRNL);
    // Disable output processing:
    raw.c_oflag &= ~(OPOST);
    // Make read() return immediately with whatever is available:
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    // Ensure we restore on exit:
    std::atexit([](){
      tcsetattr(STDIN_FILENO, TCSANOW, &__orig_termios);
    });

    __raw_mode_enabled = true;
  }

  // Non-blocking poll. Returns -1 if no key. Otherwise returns unsigned char value (0..255).
  inline int getCharNonBlocking() {
    // Use select() with 0 timeout to poll stdin.
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);
    struct timeval tv = {0, 0}; // zero timeout = poll
    int ret = select(STDIN_FILENO + 1, &rfds, nullptr, nullptr, &tv);
    if (ret > 0 && FD_ISSET(STDIN_FILENO, &rfds)) {
      unsigned char c;
      ssize_t n = read(STDIN_FILENO, &c, 1);
      if (n == 1) return (int)c;
    }
    return -1;
  }
#endif
