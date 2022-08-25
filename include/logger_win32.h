/**
@file logger_win32.h
*/

#ifdef _WIN32

#include <iostream>
#include <windows.h>

#define RED 12
#define YELLOW 14

template <typename T> void cwriteOS(LOG_TYPE logType, const T &buffer) {
  if (logType < ERR || logType > WARN)
    logType = INFO;
  HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  switch (logType) {
  case ERR:
    SetConsoleTextAttribute(consoleHandle, RED);
    break;
  case INFO:
    break;
  case WARN:
    SetConsoleTextAttribute(consoleHandle, YELLOW);
    break;
  }
  std::cout << buffer;
}

#endif
