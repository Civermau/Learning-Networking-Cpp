#include <cstdarg>
#include <cstring>
#include <iostream>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

// Returns the size of the console as (width, height)
// The 'inline' keyword allows this function to be safely included in header
// files without causing multiple definition errors during linking. It also
// hints to the compiler that the function should be expanded in-place for
// better performance, eliminating function call overhead for this small,
// frequently-used function.
inline std::pair<int, int> getConsoleSize() {
#ifdef _WIN32
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  int columns = 0, rows = 0;
  HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  if (GetConsoleScreenBufferInfo(hStdout, &csbi)) {
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
  }
  return {columns, rows};
#else
  struct winsize w;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
    return {w.ws_col, w.ws_row};
  }
  return {0, 0};
#endif
}

inline void setCursorPosition(int x, int y) {
#ifdef _WIN32
  // Windows version
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  COORD position = {(SHORT)x, (SHORT)y};
  SetConsoleCursorPosition(hConsole, position);
#else
  // ANSI escape sequence for Linux/macOS
  std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H";
#endif
}

// Prints a numbered list of options to the console.
// Usage: printOptionsList("Host", "Join");
//        printOptionsList("Kick", "Quit", "Resume");
inline void printOptionsList(int count, ...) {
  auto [x, y] = getConsoleSize();
  int startRow = (y / 2) - (count / 2);
  // int selection = 0;

  va_list args;
  va_start(args, count);

  for (int i = 0; i < count; ++i) {
    const char *option = va_arg(args, const char *);
    int len = std::strlen(option);
    setCursorPosition((x / 2) - (len / 2), startRow + i);
    // std::string box = selection == i ? "[*]" : "[ ]"; 
    // std::cout << box << ". " << option << std::endl;
    std::cout << i +1  << ". " << option << std::endl;
  }
  va_end(args);
}

inline void hideCursor() {
#ifdef _WIN32
#include <windows.h>
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_CURSOR_INFO cursorInfo;
  GetConsoleCursorInfo(hConsole, &cursorInfo);
  cursorInfo.bVisible = FALSE;
  SetConsoleCursorInfo(hConsole, &cursorInfo);
#else
  // ANSI escape code to hide cursor
  std::cout << "\033[?25l";
#endif
}

inline void clearScreen() {
#ifdef _WIN32
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD cellCount;
  DWORD count;
  COORD homeCoords = {0, 0};

  if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    return;

  cellCount = csbi.dwSize.X * csbi.dwSize.Y;

  FillConsoleOutputCharacter(hConsole, (TCHAR)' ', cellCount, homeCoords,
                             &count);

  FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords,
                             &count);

  SetConsoleCursorPosition(hConsole, homeCoords);
#else
  // ANSI escape sequences for Linux/macOS
  std::cout << "\033[2J\033[H";
#endif
}