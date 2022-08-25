/**
@file logger_ansi.h
*/

#ifdef __gnu_linux__

#include <iostream>

#define RED "\x1B[31;1m"
#define YELLOW "\x1B[33;1m"
#define RESET "\x1B[0m"

template <typename T> void cwriteOS(LOG_TYPE logType, const T &buffer) {
  if (logType < ERR || logType > WARN)
    logType = INFO;
  switch (logType) {
  case ERR:
    std::cout << RED;
    break;
  case INFO:
    break;
  case WARN:
    std::cout << YELLOW;
    break;
  }
  std::cout << buffer << RESET;
}

#endif
