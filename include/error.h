#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void fatal_error(const char *fmt, ...);
void warning(const char *fmt, ...);

#endif /* __ERROR_H__ */
