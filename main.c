#include <stdlib.h>
#include <string.h>

#include "net.h"
#include "socket.h"
#include "sndup.h"
#include "error.h"
#include "print.h"
#include "file.h"

int main(int argc, char *argv[]) {
	char *ip_and_port;
	char *cmd_file, *cmd_string, *cmd;

	if (argc != 3)
		fatal_error("usage: %s <IP address:port> <command file>\n", argv[0]);

	// initialize the network connection
	ip_and_port = argv[1];
	socket_init(ip_and_port);

	// read the command file
	cmd_file = file_read_to_char_buf(argv[2]);
	cmd_string = cmd_file;
	while((cmd = strsep(&cmd_string, "\n\r")) != NULL) {
		char *buf;
		if (*cmd == '\0') continue; // blank line
		if (*cmd == '#') continue; // comment line
		buf = sndup("%s\n", cmd);
		socket_send((unsigned char *)buf, strlen(buf));
		free(buf);
	}
	free(cmd_file);

	// cleanup socket
	socket_destroy();

	exit(EXIT_SUCCESS);
}
