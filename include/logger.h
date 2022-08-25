/**
@file logger.h
*/

#pragma once

#ifdef __cplusplus

#include <fstream>
#include <string>
#include <type_traits>

#endif

#define ERR FHM_ERR
#define WARN FHM_WARN
#define INFO FHM_INFO

#define SUCCESS FHM_SUCCESS
#define UNSUPPORTED_LOG_TYPE FHM_UNSUPPORTED_LOG_TYPE
#define NO_FILE_SUPPLIED FHM_NO_FILE_SUPPLIED
#define INVALID_PATH FHM_INVALID_PATH
#define INVALID_ARG FHM_INVALID_ARG

#define logger fhm_logger

typedef enum LOG_TYPE { FHM_ERR, FHM_INFO, FHM_WARN } LOG_TYPE;

typedef enum LOG_RETURN_CODE {

  FHM_SUCCESS,
  FHM_UNSUPPORTED_LOG_TYPE,
  FHM_NO_FILE_SUPPLIED,
  FHM_INVALID_PATH,
  FHM_INVALID_ARG

} LOG_RETURN_CODE;

typedef struct FHM_Logger {

  /** Sets the file to be used for file logging, can be safely changed at any
   * time */
  LOG_RETURN_CODE (*setFile)(const char *path);
  /// Logs to a file supplied to setFile
  /** Note that due to the nature of C variadic functions you must make sure to
   * pass the correct number and type of arguments otherwise it is UB */
  LOG_RETURN_CODE (*flog)(const char *format, ...);
  /// Logs to the console in color depending on the LOG_TYPE
  /** Note that due to the nature of C variadic functions you must make sure to
   * pass the correct number and type of arguments otherwise it is UB */
  LOG_RETURN_CODE (*clog)(LOG_TYPE logType, const char *format, ...);

#ifdef __cplusplus

  /** Logs to the console in color depending on the LOG_TYPE. Supports user
   * defined types with an overloaded operator<< */
  template <typename T> friend LOG_TYPE operator<<(LOG_TYPE, const T &);

  /** Logs to a file supplied to setFile. Does not support user defined types
   * with an overloaded operator<<*/
  template <typename T, typename enableif>
  friend const FHM_Logger &operator<<(const FHM_Logger &, const T &);

#endif

} FHM_Logger;

#ifdef __cplusplus
extern "C" {
#endif

extern const FHM_Logger fhm_logger;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include "logger_ansi.h"
#include "logger_win32.h"

/** Logs to the console in color depending on the LOG_TYPE. Supports user
 * defined types with an overloaded operator<< */
template <typename T> LOG_TYPE operator<<(LOG_TYPE logType, const T &t) {
  cwriteOS(logType, t);
  return logType;
}

/** Allows chaining calls */
LOG_TYPE operator<<(LOG_TYPE logTypeBase, LOG_TYPE logTypeNew);

void fwriteHelper(const std::string &);

template <typename T>
using enableIfString = typename std::enable_if<
    std::is_same<T, std::string>::value || std::is_same<T, char *>::value ||
        std::is_same<T, const char *>::value ||
        std::is_same<typename std::remove_extent<T>::type, char>::value,
    bool>::type;

template <typename T>
using enableIfNum =
    typename std::enable_if<(std::is_integral<T>::value ||
                             std::is_floating_point<T>::value) &&
                                !std::is_same<T, char>::value,
                            bool>::type;

/** Logs to a file supplied to setFile. Does not support user defined types
 * with an overloaded operator<<*/
template <typename T, enableIfString<T> = true>
const FHM_Logger &operator<<(const FHM_Logger &log, const T &t) {
  fwriteHelper(t);
  return log;
}

/** Logs to a file supplied to setFile. Does not support user defined types
 * with an overloaded operator<<*/
template <typename T, enableIfNum<T> = true>
const FHM_Logger &operator<<(const FHM_Logger &log, const T &t) {
  std::string arg = std::to_string(t);
  fwriteHelper(arg);
  return log;
}

#endif
