#pragma once
#include "defs.h"

#ifdef __cplusplus
#include <fstream>
#include <string>
#include <type_traits>
#endif

typedef enum {
	COPTL_ERR,
	COPTL_WARN,
	COPTL_INFO
} COPTL_CLOG_TYPE;

typedef enum {
	COPTL_SUCCESS,
	COPTL_UNSUPPORTED_LOG_TYPE,
	COPTL_NO_FILE_SUPPLIED,
	COPTL_INVALID_PATH,
	COPTL_INVALID_ARG
} COPTL_RETURN_CODE;

typedef struct COPTL_Logger {
	/* Sets the file to be used for file logging, can be safely changed at any time */
	COPTL_RETURN_CODE (*setFile)
	(const char* path);
	/* Note that due to the nature of C variadic functions you must make sure to
   * pass the correct number and type of arguments otherwise it is UB */
	COPTL_RETURN_CODE (*flog)
	(const char* prefix, const char* format, ...);
	/** Note that due to the nature of C variadic functions you must make sure to
   * pass the correct number and type of arguments otherwise it is UB */
	COPTL_RETURN_CODE (*clog)
	(COPTL_CLOG_TYPE logType, const char* format, ...);

#ifdef __cplusplus
	/* Supports user defined types with an overloaded operator<< */
	template <typename T>
	friend COPTL_CLOG_TYPE operator<<(COPTL_CLOG_TYPE, const T&);

	/* Does not support user defined types with an overloaded operator<< */
	template <typename T, typename enableif>
	friend const std::string operator<<(std::string&, const T&);
#endif
} COPTL_Logger;

#ifdef __cplusplus
extern "C" {
#endif

extern const COPTL_Logger coptl_logger;

const char* coptl_concatDbgInfoC(const char* funcName, int lineNum, const char* fileName);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include "logger_ansi.h"
#include "logger_win32.h"

std::string coptl_concatDbgInfoCpp(const char* funcName, int lineNum, const char* fileName);

template <typename T>
COPTL_CLOG_TYPE operator<<(COPTL_CLOG_TYPE logType, const T& type)
{
	cwriteOS(logType, type);
	return logType;
}

/* Allows chaining calls */
COPTL_CLOG_TYPE operator<<(COPTL_CLOG_TYPE logTypeBase, COPTL_CLOG_TYPE logTypeNew);

void fwriteHelper(const std::string& arg);

template <typename T>
using enableIfString = typename std::enable_if<
	std::is_same<T, std::string>::value || std::is_same<T, char*>::value || std::is_same<T, const char*>::value
		|| std::is_same<typename std::remove_extent<T>::type, char>::value,
	bool>::type;

template <typename T>
using enableIfNum =
	typename std::enable_if<(std::is_integral<T>::value || std::is_floating_point<T>::value)
			&& !std::is_same<T, char>::value,
		bool>::type;

template <typename T, enableIfString<T> = true>
std::string operator<<(const std::string& prefix, const T& type)
{
	fwriteHelper(prefix + type);
	return "";
}

template <typename T, enableIfNum<T> = true>
std::string operator<<(const std::string& prefix, const T& type)
{
	fwriteHelper(prefix + std::to_string(type));
	return "";
}
#endif
