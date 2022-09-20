/**
@file logger_win32.h
*/

#ifdef _WIN32

#include <iostream>
#include <windows.h>

#define COPTL_RED 12
#define COPTL_YELLOW 14

template <typename T> void cwriteOS(COPTL_CLOG_TYPE logType, const T &buffer) {
  if (logType < COPTL_ERR || logType > COPTL_INFO)
    logType = COPTL_INFO;
  HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  switch (logType) {
  case COPTL_ERR:
    SetConsoleTextAttribute(consoleHandle, COPTL_RED);
    break;
  case COPTL_WARN:
    SetConsoleTextAttribute(consoleHandle, COPTL_YELLOW);
    break;
  case COPTL_INFO:
    break;
  }
  std::cout << buffer;
}

#endif
