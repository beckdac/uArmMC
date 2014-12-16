#include "socket.h"

static struct socket_struct sock;

void socket_init(char *ip_and_port) {
	int len, i;

	if (!ip_and_port)
		fatal_error("socket_init: ip_and_port was NULL, aborting...\n");
	len = strlen(ip_and_port);

	memset(&sock, 0, sizeof(struct socket_struct));

	sock.host = strdup(ip_and_port);
	sock.port = DEFAULT_PORT;

	for (i = 0; i < len; ++i) {
		if (ip_and_port[i] == ':') {
			sock.host[i] = '\0';
			sock.port = atoi(&sock.host[i + 1]);
			break;
		}
	}
	print("trying to open TCP socket to host %s on port %d\n", sock.host, sock.port);

	/* open connection */
	sock.sock = net_tcp_cli_init(sock.host, sock.port, &sock.servaddr);
	if (sock.sock == -1)
		fatal_error("socket_init: unable to open TCP connection to host '%s' on port %d\n", sock.host, sock.port);
}

void socket_send(unsigned char *buff, unsigned int bytes) {
	int sent = net_send(sock.sock, buff, bytes);
	if (sent != bytes) {
		fatal_error("socket_send: net_send failed for TCP connection to host '%s' on port %d\n", sock.host, sock.port);
	}
}

void socket_receive(unsigned char *buff, unsigned int bytes) {
	int recv;

	memset(buff, 0, bytes);

	recv = net_recv(sock.sock, buff, bytes);
	if (recv != bytes) {
		fatal_error("socket_receive: net_send failed for TCP connection to host '%s' on port %d\n", sock.host, sock.port);
	}
}

void socket_destroy(void) {
	net_close(sock.sock);
}
