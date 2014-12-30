#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>

#include "net.h"
#include "socket.h"
#include "error.h"
#include "print.h"

#include "cmd.h"
#include "km.h"
#include "calibrate.h"

int main(int argc, char *argv[]) {
	char *task = NULL, *socket = NULL, *device = NULL, *baud = NULL, *in_file = NULL;
	int c;

	opterr = 0;

	while ((c = getopt(argc, argv, "t:s:d:b:i:h")) != -1) {
		switch (c) {
			case 'h':
USAGE:
				printf("usage: %s -t <task> [ [-s <socket>] [-d <device> ] [-b <baud>] [-i <input file>] ]\n", argv[0]);
				printf("\t\ttask\ttask name, one of:\n");
				printf("\t\t\t\tbatch\tsend commands via socket or serial device\n");
				printf("\t\t\t\tkm_test\tperform tests of the kinematics code\n");
				printf("\t\tsocket\tuse a TCP socket to connect to arm\n");
				printf("\t\t\t\tin the form: 192.168.1.20:23 (IP:port)\n");
				printf("\t\tdevice\tuse a serial device to connect to arm\n");
				printf("\t\tbaud\tuse specified baud rate for serial device\n");
				printf("\t\tinput\ttake input from file or '-' for STDIN\n");
				printf("\n\tNOTE: specify either socket or device.  If using device, must specify baud rate\n");
				exit(EXIT_FAILURE);
				break;
			case 't':
				task = strdup(optarg);
				break;
			case 's':
				socket = strdup(optarg);
				break;
			case 'd':
				device = strdup(optarg);
				break;
			case 'b':
				baud = strdup(optarg);
				break;
			case 'i':
				in_file = strdup(optarg);
				break;
			case '?':
				if (optopt == 't' || optopt == 's' || optopt == 'd' || optopt == 'b' || optopt == 'i') {
						fprintf(stderr, "option -%c requires an argument\n", optopt);
				} else if (isprint(optopt)) {
						fprintf(stderr, "unknown option `-%c'.\n", optopt);
				} else {
						fprintf(stderr, "unknown option character `\\x%x'.\n", optopt);
				}
				goto USAGE;
				break;
			default:
				goto USAGE;
		}
	}

	if (strcmp(task, "batch") == 0) {
		if (!socket && !device) {
			fprintf(stderr, "please specify either socket or device\n");
			goto USAGE;
		} else if (socket && device) {
			fprintf(stderr, "please specify either socket or device, not both\n");
			goto USAGE;
		} else if (socket) {
			if (!socket_init(socket)) {
				fatal_error("failed to open command channel via socket: %s\n", socket);
			}
		} else if (device && !baud) {
			fprintf(stderr, "please specify a baud rate for the serial device\n");
			goto USAGE;
		} else { // device
			fatal_error("serial device handling is planned but not currently supported\n");
		}
		if (!cmd_prepare_channel()) {
			fatal_error("%s: failed to prepare command channel\n", __PRETTY_FUNCTION__);
		}
		cmd_batch(in_file);
		socket_destroy();
	} else if (strcmp(task, "km_test") == 0) {
		km_test();
	} else {
		fprintf(stderr, "unknown task '%s'\n", task);
		goto USAGE;
	}
	exit(EXIT_SUCCESS);
}
