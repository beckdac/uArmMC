#include "net.h"
#include "error.h"

int net_tcp_cli_init(char *hostname, int port, struct sockaddr_in *servaddr) {
	int sock;
	struct hostent *host;

	/* create a new socket */
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		warning("net_tcp_cli_init: unable to create a new socket when attempting to connect to '%s':\n\t%s\n", hostname, strerror(errno));
                return -1;
        }
	/* resolve host name */
	if ((host = gethostbyname(hostname)) == NULL) {
		warning("net_tcp_cli_init: unable to get host information for '%s':\n\t%s\n", hostname, hstrerror(h_errno));
		return -1;
	}
	/* set up socket address structures */
        memset(servaddr,  0 , sizeof(struct sockaddr_in));
        servaddr->sin_family = AF_INET;
        servaddr->sin_port = htons(port);
	servaddr->sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)host->h_addr)));
	/* initiate a connection */
        if (connect(sock, (struct sockaddr *)servaddr, sizeof(struct sockaddr)) == -1) {
                warning("net_tcp_cli_init: connect error for host '%s' on port %d:\n\t%s\n", hostname, port, strerror(errno));
                return -1;
        }

	return sock;
}

int net_tcp_ser_init(int port, struct sockaddr_in *servaddr) {
	int sock, opt;

	/* create a new socket */
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		warning("net_tcp_ser_init: unable to create a new socket for service on port %d:\n\t%s\n", port, strerror(errno));
                return -1;
        }
	opt = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(int)) < 0){
		warning("net_tcp_ser_init: setsockopt REUSEADDR failed for service on port %d:\n\t%s\n", port, strerror(errno));
                return -1;
	}
	/* set up socket address structures */
        memset(servaddr,  0 , sizeof(struct sockaddr_in));
        servaddr->sin_family = AF_INET;
	servaddr->sin_addr.s_addr = INADDR_ANY;
        servaddr->sin_port = htons(port);
	/* bind the socket to the port */
	if (bind(sock, (struct sockaddr *)servaddr, sizeof(struct sockaddr_in)) == -1) {
		warning("net_tcp_ser_init: bind error for service on port %d:\n\t%s\n", port, strerror(errno));
		return -1;
	}
	/* begin to listen on the port */
	if (listen(sock, NET_TCP_DEFAULT_BACKLOG) == -1) {
		warning("net_tcp_ser_init: listen error for service on port %d:\n\t%s\n", port, strerror(errno));
		return -1;
	}

	return sock;
}

int net_tcp_ser_accept(int listen_sock, struct sockaddr_in *servaddr, struct sockaddr_in *cliaddr) {
	int sock;

	size_t addrlen = sizeof(struct sockaddr_in);
	memset(cliaddr,  0, addrlen);

	/* accept the connection */
	if ((sock = accept(listen_sock, (struct sockaddr *)cliaddr, (socklen_t *)&addrlen)) == -1) {
		warning("net_tcp_ser_init: accept connection error for service on port %d:\n\t%s\n", ntohl(servaddr->sin_port), strerror(errno));
		return -1;
	}
	return sock;
}

int net_udp_cli_init(char *hostname, int port, struct sockaddr_in *servaddr) {
	int sock;
	struct hostent *host;

	/* create a new socket */
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		warning("net_udp_cli_init: unable to create a new socket when attempting to connect to '%s':\n\t%s\n", hostname, strerror(errno));
                return -1;
	}
	/* resolve host name */
	if ((host = gethostbyname(hostname)) == NULL) {
		warning("net_udp_cli_init: unable to get host information for '%s':\n\t%s\n", hostname, hstrerror(h_errno));
		return -1;
	}
	/* set up socket address structures */
	memset(servaddr, 0, sizeof(struct sockaddr_in));
	servaddr->sin_family = AF_INET;
	servaddr->sin_port = htons(port);
	servaddr->sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)host->h_addr)));

	return sock;
}

int net_send(int sock, void *buf, size_t len) {
	int remaining = len;
	int sent = 0;
	int bytes;

        while (sent < len) {
            bytes = send(sock, (void *)((char *)buf + sent), remaining, 0);
            if (bytes == -1)
		break;
            sent += bytes;
            remaining -= bytes;
        }

        return (bytes == -1 ? -1 : sent);
} 

int net_recv(int sock, void *buf, size_t len) {
	int remaining = len;
	int received = 0;
	int bytes;

	while (received < len) {
		bytes = recv(sock, (void *)((char *)buf + received), remaining, 0);
		if (bytes == -1 || bytes == 0)
			break;
		remaining -= bytes;
		received += bytes;
	}
	return (bytes == -1 ? -1 : received);
}

int net_close(int sock) {
	return close(sock);
}
