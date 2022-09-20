# COPTL - C/C++ Optimal Platform-agnostic Thread-safe Logger
COPTL is a flexible logger usable in C and C++ projects.
## Usage
### C console logging
```
int foo = 5;
logger.clog(I, "foo: %d\n", foo);
```
The first argument is the log type which is one of:
1. COPTL_INFO or I if not defined.
2. COPTL_ERR or E if not defined.
3. COPTL_WARN or W if not defined.

The same rules as printf are used when it comes to formatting. Currently the following formats are supported:
- %c
- %s
- %d/%i
- %f
- %l
- %lu
- %ll
- %llu
- %u
- %p
### C file logging
```
logger.setFile("log_file.txt");
int foo = 5;
logger.flog(F, "foo: %d\n", foo);
```
The first parameter is the log type which is one of:
1. COPTL_F or F if not defined.
2. COPTL_F_DBG or F_DBG if not defined, used for outputting additional information such as function name, line number and file name.

The same rules as above for formatting apply.
### C++ console logging
1. Same as with C i.e. logger.clog.
2. C++ specific:
```
I<<"foo"<<5<<'\n';
```
Where the first argument is the log type, one of the same three stated in the C section.
You can also chain log types:
```
E<<"Error\n"<<W<<"Warning\n";
```
### C++ file logging
1. Same as with C i.e. logger.flog with the following difference:
COPTL_F_DBG_C instead of F_DBG i.e. F_DBG_C instead of F_DBG
2. C++ specific
```
logger.setFile("/home/user/project/log_file.txt")
F<<"foo"<<5<<'\n';
```
Where the first argument is the log type, one of the two stated in the C section.
You cannot chain log types in this case.
## Build
Use add_subdirectory in your own cmake and link the library target **coptl**.
