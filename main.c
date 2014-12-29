#include <stdlib.h>
#include <string.h>

#include "net.h"
#include "socket.h"
#include "sndup.h"
#include "error.h"
#include "print.h"
#include "file.h"

#include "cmd.h"
#include "ik.h"

#define MAX_BUF_LEN 1024
char inbuf[MAX_BUF_LEN];

int main(int argc, char *argv[]) {
	char *ip_and_port;
	char *cmd_file, *cmd_string, *cmd;

	if (argc != 3)
		fatal_error("usage: %s <IP address:port> <'iktest'|command file>\n", argv[0]);

	// initialize the network connection
	ip_and_port = argv[1];
	if (!socket_init(ip_and_port)) {
		fatal_error("%s: failed to open command channel\n", __PRETTY_FUNCTION__);
	}
	if (!cmd_prepare_channel()) {
		fatal_error("%s: failed to prepare command channel\n", __PRETTY_FUNCTION__);
	}

	if (strcmp(argv[2], "iktest") == 0) {

		ik_test();

		socket_destroy();

		exit(EXIT_SUCCESS);
	}

	// read the command file
	cmd_file = file_read_to_char_buf(argv[2]);
	cmd_string = cmd_file;
	while((cmd = strsep(&cmd_string, "\n\r")) != NULL) {
		char *buf;
		if (*cmd == '\0') continue; // blank line
		if (*cmd == '#') continue; // comment line
		buf = sndup("%s\n", cmd);
		cmd_execute(buf, strlen(buf));
		free(buf);
	}
	free(cmd_file);

	// cleanup socket
	socket_destroy();

	exit(EXIT_SUCCESS);
}
