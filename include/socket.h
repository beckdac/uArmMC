#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "print.h"
#include "net.h"

#define DEFAULT_PORT 2000

void socket_init(char *ip_and_port);
void socket_send(unsigned char *buff, unsigned int bytes);
void socket_receive(unsigned char *buff, unsigned int bytes);
void socket_destroy(void);

struct socket_struct {
	char	*host;					/* name of remote host */
	int	port;					/* port # of service on remote host */
	int	sock;					/* socket address of remote host for idb_data */
	struct sockaddr_in servaddr;			/* server address information of remote host */
};


#endif /* _SOCKET_H_ */
