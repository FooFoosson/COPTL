/**
@file logger_ansi.h
*/

#ifdef __gnu_linux__

#include <iostream>

#define COPTL_RED "\x1B[31;1m"
#define COPTL_YELLOW "\x1B[33;1m"
#define COPTL_RESET "\x1B[0m"

template <typename T> void cwriteOS(COPTL_CLOG_TYPE logType, const T &buffer) {
  if (logType < COPTL_ERR || logType > COPTL_INFO)
    logType = COPTL_INFO;
  switch (logType) {
  case COPTL_ERR:
    std::cout << COPTL_RED;
    break;
  case COPTL_WARN:
    std::cout << COPTL_YELLOW;
    break;
  case COPTL_INFO:
    break;
  }
  std::cout << buffer << COPTL_RESET;
}

#endif
