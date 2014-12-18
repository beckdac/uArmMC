#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "print.h"
#include "net.h"

#define DEFAULT_PORT 2000

#define SOCKET_RECV_TIMEOUT_MS 2000

int socket_init(char *ip_and_port);
int socket_set_recv_timeout(unsigned int timeout_ms);
int socket_set_send_timeout(unsigned int timeout_ms);
int socket_send(unsigned char *buff, unsigned int bytes);
int socket_receive(unsigned char *buff, unsigned int bytes);
void socket_destroy(void);

struct socket_struct {
	char	*host;					/* name of remote host */
	int	port;					/* port # of service on remote host */
	int	sock;					/* socket address of remote host for idb_data */
	struct sockaddr_in servaddr;			/* server address information of remote host */
};


#endif /* _SOCKET_H_ */
