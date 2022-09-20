#include "logger.h"
#include <algorithm>
#include <fstream>
#include <ios>
#include <limits>
#include <stdarg.h>
#include <string>
#include <type_traits>

class FileStream {
public:
	FileStream();
	std::ofstream* stream;
	std::string buffer;
	~FileStream();
};

FileStream::FileStream()
	: stream(0)
{
	/* Gcc's standard cpp library has a maximum internal buffer size of 1024 */
	/* so to maximize performance buffer exactly that size plus the null
   * terminator which is not written */
	/* Microsoft's standard cpp library has no such limitation and we can call
   * pubsetbuf to set the buffer */
	/* but to avoid virtual dispatch on every write call it is more efficient to
   * buffer on our side */
	buffer.reserve(1025);
};

FileStream::~FileStream()
{
	if (stream) {
		/* We cannot support user defined types since the ofstream::operator<< is
     * not templated */
		/* It is only overloaded for certain types such as integral types*/
		stream->write(buffer.data(), buffer.size());
		delete stream;
	}
}

static thread_local FileStream fstream;

static COPTL_RETURN_CODE setFile(const char* path)
{
	if (!path)
		return COPTL_NO_FILE_SUPPLIED;
	/* Parameterized constructor opens the file and appends the supplied mode to
   * ios_base::out */
	/* Will not fail if an already open file is opened again */
	std::ofstream* newStream = new std::ofstream(path, std::ios_base::app);
	if (!newStream->is_open()) {
		delete newStream;
		return COPTL_INVALID_PATH;
	}
	if (fstream.stream) {
		fstream.stream->write(fstream.buffer.data(), fstream.buffer.size());
		fstream.buffer.clear();
		/* Destructor closes the file and thusly flushes the buffer */
		delete fstream.stream;
	}
	fstream.stream = newStream;
	/* TODO: write the current datetime */
	return COPTL_SUCCESS;
}

static std::string numToHex(unsigned long long arg)
{
	std::string res = "";
	do {
		char digit = arg % 16;
		arg /= 16;
		if (digit > 9 && digit < 16)
			res += digit - 10 + 'a';
		else
			res += digit + '0';
	} while (arg > 0);
	std::reverse(res.begin(), res.end());
	return res;
}

static COPTL_RETURN_CODE flog(const char* prefix, const char* format, ...)
{
	if (!fstream.stream)
		return COPTL_NO_FILE_SUPPLIED;
	/* Standard C headers already include guarded extern "C" declarations so no
   * need for prefixing ourselves */
	va_list va_ptr;
	va_start(va_ptr, format);
	std::string formattedArg(prefix);
	formattedArg.reserve(formattedArg.size() + std::numeric_limits<double>::digits10 + 1 /* Null terminator*/);

	for (const char* current = format; *current != '\0'; ++current) {
		if (*current == '%')
			if (*(current + 1) != '\0') {
				switch (*(++current)) {
				case 'c':
					formattedArg += (char)va_arg(va_ptr, int);
					break;
				case 's': {
					char* c = (char*)va_arg(va_ptr, void*);
					formattedArg.append(c);
				} break;
				case 'd':
				case 'i':
					formattedArg += std::to_string(va_arg(va_ptr, int));
					break;
				case 'f':
					formattedArg += std::to_string(va_arg(va_ptr, double));
					break;
				case 'l':
					switch (*(current + 1)) {
					case '\0':
						formattedArg += std::to_string(va_arg(va_ptr, long int));
						break;
					case 'l':
						++current;
						if (*(current + 1) != '\0' && *(current + 1) == 'u') {
							formattedArg += std::to_string(va_arg(va_ptr, unsigned long long int));
							++current;
						} else
							formattedArg += std::to_string(va_arg(va_ptr, long long int));
						break;
					case 'u':
						formattedArg += std::to_string(va_arg(va_ptr, unsigned long int));
						++current;
						break;
					default:
						formattedArg += std::to_string(va_arg(va_ptr, long int));
					}
					break;
				case 'u':
					formattedArg += std::to_string(va_arg(va_ptr, unsigned int));
					break;
				case 'p': {
					unsigned long long arg = (unsigned long long)(va_arg(va_ptr, void*));
					formattedArg += numToHex(arg);
				} break;
				default:
					fstream.buffer.clear();
					return COPTL_INVALID_ARG;
				}
				/* Reduce memory footprint */
				unsigned int remainingCapacity = fstream.buffer.capacity() - 1 /* Null terminator */ - fstream.buffer.size();
				if (formattedArg.size() > remainingCapacity) {
					fstream.buffer += formattedArg.substr(0, remainingCapacity);
					fstream.stream->write(fstream.buffer.data(), fstream.buffer.size());
					/* Efficient since it does not change the underlying array but merely
           * modifies size */
					/* So by accessing data() on std::string with size() we will get valid
           * input for out stream */
					fstream.buffer.clear();
					fstream.buffer += formattedArg.substr(
						remainingCapacity, formattedArg.size() - remainingCapacity);
				} else
					fstream.buffer += formattedArg;
				formattedArg.clear();
				continue;
			}
		fstream.buffer += *current;
	}

	va_end(va_ptr);
	return COPTL_SUCCESS;
}

static COPTL_RETURN_CODE clog(COPTL_CLOG_TYPE logType, const char* format, ...)
{
	if (logType < E || logType > W)
		return COPTL_UNSUPPORTED_LOG_TYPE;
	va_list va_ptr;
	va_start(va_ptr, format);
	std::string cbuffer;
	cbuffer.reserve(256);

	for (const char* current = format; *current != '\0'; ++current) {
		if (*current == '%') {
			if (*(current + 1) != '\0')
				switch (*(++current)) {
				case 'c':
					cbuffer += (char)va_arg(va_ptr, int);
					break;
				case 's': {
					char* c = (char*)va_arg(va_ptr, void*);
					while (*c != '\0')
						cbuffer += *(c++);
				} break;
				case 'd':
				case 'i':
					cbuffer += std::to_string(va_arg(va_ptr, int));
					break;
				case 'f':
					cbuffer += std::to_string(va_arg(va_ptr, double));
					break;
				case 'l':
					switch (*(current + 1)) {
					case '\0':
						cbuffer += std::to_string(va_arg(va_ptr, long int));
						break;
					case 'l':
						++current;
						if (*(current + 1) != '\0' && *(current + 1) == 'u') {
							cbuffer += std::to_string(va_arg(va_ptr, unsigned long long int));
							++current;
						} else
							cbuffer += std::to_string(va_arg(va_ptr, long long int));
						break;
					case 'u':
						cbuffer += std::to_string(va_arg(va_ptr, unsigned long int));
						++current;
						break;
					case 'p': {
						unsigned long long arg = (unsigned long long)(va_arg(va_ptr, void*));
						cbuffer += numToHex(arg);
					} break;
					default:
						cbuffer += std::to_string(va_arg(va_ptr, long int));
					}
					break;
				case 'u':
					cbuffer += std::to_string(va_arg(va_ptr, unsigned int));
					break;
				default:
					return COPTL_INVALID_ARG;
				}
		} else
			cbuffer += *current;
	}

	/* We do not buffer since in the case of windows we can't append a sequence of
   * characters */
	/* to the buffer to change the text color, we have to make a call to the win32
   * uapi */
	/* which makes buffering nigh impossible, also since the standard output is
   * shared */
	/* between threads and processes the timing might be important */
	cwriteOS(logType, cbuffer);
	va_end(va_ptr);
	return COPTL_SUCCESS;
}

COPTL_CLOG_TYPE operator<<(COPTL_CLOG_TYPE logTypeBase, COPTL_CLOG_TYPE logTypeNew)
{
	return logTypeNew;
}

const char* coptl_concatDbgInfoC(const char* funcName, int lineNum, const char* fileName)
{
	std::string res = "\nFUNCTION NAME: ";
	res.reserve(45 + 20);
	res += funcName;
	res += ", LINE NUMBER: ";
	res += std::to_string(lineNum);
	res += ", FILE NAME: ";
	res += fileName;
	res += "\n";
	size_t resSize = res.size() + 1;
	char* resLiteral = new char[resSize];
	res.copy(resLiteral, resSize);
	return resLiteral;
}

std::string coptl_concatDbgInfoCpp(const char* funcName, int lineNum, const char* fileName)
{
	std::string res = "\nFUNCTION NAME: ";
	res.reserve(45 + 20);
	res += funcName;
	res += ", LINE NUMBER: ";
	res += std::to_string(lineNum);
	res += ", FILE NAME: ";
	res += fileName;
	res += "\n";
	return res;
}
void fwriteHelper(const std::string& arg)
{
	if (!fstream.stream) {
		W << "Logger: Writing to a file without first calling setFile will "
				"result in no data being written!\n";
		return;
	}
	unsigned int remainingCapacity = fstream.buffer.capacity() - 1 - fstream.buffer.size();
	if (arg.size() > remainingCapacity) {
		fstream.buffer += arg.substr(0, remainingCapacity);
		fstream.stream->write(fstream.buffer.data(), fstream.buffer.size());
		fstream.buffer.clear();
		fstream.buffer += arg.substr(remainingCapacity, arg.size() - remainingCapacity);
	} else
		fstream.buffer += arg;
}

const COPTL_Logger coptl_logger = { setFile, flog, clog };
