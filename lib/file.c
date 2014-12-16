#include "file.h"
#include "error.h"

char *file_read_to_char_buf(char *filename) {
	FILE *fp;
        char *buf = NULL;
        int buf_len = 0, c, l;

	if ((fp = fopen(filename, "r")) == NULL) {
		fatal_error("file_read_to_char_buf: unable to open file '%s'\n", filename);
	}

        buf = (char *)malloc(sizeof(char));
        buf[0] = '\0';
        buf_len = 1;

        while ((c = getc(fp)) != EOF) {
                l = buf_len++;
                buf = (char *)realloc(buf, buf_len * sizeof(char));
                buf[l - 1] = c;
                buf[l] = '\0';
        }

	fclose(fp);

        return buf;
}
