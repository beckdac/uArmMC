#include "error.h"
#include "sndup.h"

void fatal_error(const char * fmt, ...) {
	va_list vargs;
	va_start(vargs, fmt);
	vfprintf(stderr, fmt, vargs);
	va_end(vargs);

	exit(EXIT_FAILURE);
}

void warning(const char *fmt, ...) {
	va_list args;

	time_t now;
	char ctime_buf[64];

	int chars = strlen (fmt);
	int ret, size = (chars + 1) * sizeof(char);
	char *buf = (char *)malloc(size), *full;

	while (1) {
		va_start(args, fmt);
		ret = vsnprintf(buf, chars, fmt, args);
		va_end(args);

		if (ret > -1 && ret < chars) {
			break;
		}

		if (ret > -1) {
			chars = ret + 1;
			size = chars * sizeof(char);
		} else {
			chars *= 2;
			size = chars * sizeof(char);
		}
		buf = (char *)realloc(buf, size);
	}


	now = time(NULL);
	ctime_r(&now, ctime_buf);
	ctime_buf[strlen(ctime_buf) - 1] = '\0';

	full = sndup("%s - WARNING - %s\n", ctime_buf, buf);

	free(buf);

	fprintf(stdout, full);
	free(full);
}
