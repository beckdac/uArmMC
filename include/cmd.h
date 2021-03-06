#ifndef __CMD_H__
#define __CMD_H__

#include "socket.h"
#include "file.h"

#define MAX_RESPONSE_LEN 1024

int cmd_prepare_channel(void);
typedef enum { RECEIVING, RECEIVED_OK, RECEIVED_ERROR } cmd_receive_state_t;
int cmd_execute(char *cmd, unsigned int cmd_len);
int cmd_batch(char *cmd_file);

#endif /* __CMD_H__ */
