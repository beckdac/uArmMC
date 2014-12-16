#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "sndup.h"

char *sndup(const char *fmt, ...) {
        char    *buf;
        int     size = strlen(fmt) * sizeof(char);
        int     ret;
        va_list va_l;

        buf = (char *)malloc(size);

        while (1) {
                va_start(va_l, fmt);
                ret = vsnprintf(buf, size, fmt, va_l);
                va_end(va_l);

                if (ret > -1 && ret < size)
                        return buf;

                if (ret > -1)
                        size = ret + 1;
                else
                        size *= 2;

                buf = (char *)realloc(buf, size);
        }
}

