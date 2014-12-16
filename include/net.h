#ifndef __NET_H__
#define __NET_H__

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define NET_TCP_DEFAULT_BACKLOG	3

int net_tcp_cli_init(char *hostname, int port, struct sockaddr_in *servaddr);
int net_udp_cli_init(char *hostname, int port, struct sockaddr_in *servaddr);

int net_tcp_ser_init(int port, struct sockaddr_in *servaddr);
int net_tcp_ser_accept(int listen_sock, struct sockaddr_in *servaddr, struct sockaddr_in *cliaddr);

int net_send(int sock, void *buf, size_t len);
int net_recv(int sock, void *buf, size_t len);

int net_close(int sock);

#endif /* __NET_H__ */
