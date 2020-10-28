#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "libdeb.h"

void exit_error(const char *fmt, ...) {
	char s[MAX_ERR];
	va_list argp;
	va_start(argp, fmt);
	vsnprintf(s, MAX_ERR, fmt, argp);
	va_end(argp);
	if (errno==0)
		fprintf(stderr, "%s\n", s);
	else
		perror(s);
	exit(EXIT_FAILURE);
}
