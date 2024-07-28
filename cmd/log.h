#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define C_RES "\033[0m"
#define C_ERR "\033[1;31m"

#define LOG_ERR \
	(format, ...) fprintf(stderrm C_ERR "error: " C_RES format, __VA_ARGS__)

#endif // LOG_H
