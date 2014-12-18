#include "socket.h"

static struct socket_struct sock;

int socket_init(char *ip_and_port) {
	int len, i;

	if (!ip_and_port)
		fatal_error("%s: ip_and_port was NULL, aborting...\n", __PRETTY_FUNCTION__);
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
	if (sock.sock == -1) {
		warning("%s: unable to open TCP connection to host '%s' on port %d:\n\t%s\n", __PRETTY_FUNCTION__, sock.host, sock.port, strerror(errno));
		return 0;
	}
	return 1;
}

int socket_set_send_timeout(unsigned int timeout_ms) {
	struct timeval tv;
	int ret;

	// tv.usec maxes at 1m
	if (timeout_ms >= 1000) {
		tv.tv_sec = timeout_ms / 1000;
		tv.tv_usec = (timeout_ms % 1000) * 1000;
	} else {
		tv.tv_usec = timeout_ms * 1000;
	}

	ret = setsockopt(sock.sock, SOL_SOCKET, SO_SNDTIMEO,  (void *)&tv, sizeof(struct timeval));
	if (ret == -1) {
		warning("%s: %s\n", __PRETTY_FUNCTION__, strerror(errno));
	}
	return ret;
}

int socket_set_recv_timeout(unsigned int timeout_ms) {
	struct timeval tv;
	int ret;

	// tv.usec maxes at 1m
	if (timeout_ms >= 1000) {
		tv.tv_sec = timeout_ms / 1000;
		tv.tv_usec = (timeout_ms % 1000) * 1000;
	} else {
		tv.tv_usec = timeout_ms * 1000;
	}

	ret = setsockopt(sock.sock, SOL_SOCKET, SO_RCVTIMEO,  (void *)&tv, sizeof(struct timeval));
	if (ret == -1) {
		warning("%s: %s\n", __PRETTY_FUNCTION__, strerror(errno));
	}
	return ret;
}

int socket_send(unsigned char *buff, unsigned int bytes) {
	int sent = net_send(sock.sock, buff, bytes);
	if (sent != bytes) {
		warning("%s: failed for TCP connection to host '%s' on port %d:\n\t%s\n", sock.host, sock.port, strerror(errno));
	}
	return sent;
}

int socket_receive(unsigned char *buff, unsigned int bytes) {
	int recv;

	memset(buff, 0, bytes);

	recv = net_recv(sock.sock, buff, bytes);
	if (recv != bytes) {
		printf("%d\t%d\n", recv, bytes);
		warning("%s: failed for TCP connection to host '%s' on port %d\n", __PRETTY_FUNCTION__, sock.host, sock.port, strerror(errno));
	}
	return recv;
}

void socket_destroy(void) {
	net_close(sock.sock);
}
