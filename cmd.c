#include <sys/time.h>

#include "cmd.h"

int cmd_prepare_channel(void) {
	int ret;

	ret = socket_set_recv_timeout(SOCKET_RECV_TIMEOUT_MS);
	if (ret == -1) {
		warning("%s: failed set socket receive timeout for command channel:\n\t%s\n", __PRETTY_FUNCTION__, strerror(errno));
		return 0;
	}
	return 1;
}

char cmd_response[MAX_RESPONSE_LEN];
int cmd_response_len = 0;

int cmd_response_check(char *check) {
	int len = strlen(check);

	//printf("cmd_response = %s\n", cmd_response);
	if (cmd_response_len - len < 0) 
		return 0;
	//printf("cmd_response[%d] = %s\n", cmd_response_len - len, &cmd_response[cmd_response_len - len]);
	if (strncmp(&cmd_response[cmd_response_len - len], check, len) != 0)
		return 0;

	return 1;
}

long tvdiff_ms(struct timeval *start, struct timeval *end) {
	long msec;
	msec = (end->tv_sec - start->tv_sec) * 1000;
	msec += (end->tv_usec - start->tv_usec) / 1000;
	return msec;
}

int cmd_execute(char *cmd, unsigned int cmd_len) {
	int ret;
	cmd_receive_state_t cmd_receive_state = RECEIVING;
	struct timeval start, receive_start, end;	
	long send_ms, receive_ms, total_ms;
	cmd_response_len = 0;

	printf("sending command: %s\n", cmd);

	gettimeofday(&start, NULL);

	// send
	ret = socket_send((unsigned char *)cmd, cmd_len);
	if (ret == -1) {
		warning("%s: failed to send command: %s\n", __PRETTY_FUNCTION__, cmd);
		return -1;
	} else if (ret != cmd_len) {
		warning("%s: incomplete send of command: %s\n", __PRETTY_FUNCTION__, cmd);
		return -1;
	}
	printf("%s\n", cmd);

	gettimeofday(&receive_start, NULL);

	// receive response
//int i = 0;
	do {
		if (cmd_response_len == MAX_RESPONSE_LEN - 1) {
			warning("%s: response too long for command: %s\n", __PRETTY_FUNCTION__, cmd);
			return -1;
		}
		ret = socket_receive((unsigned char *)&cmd_response[cmd_response_len++], 1);
		if (ret == -1) {
			warning("%s: incomplete response for command: %s\n", __PRETTY_FUNCTION__, cmd);
			return -1;
		} else if (ret != 1) {
			warning("%s: unexpected response receive length for command: %s\n", __PRETTY_FUNCTION__, cmd);
			return -1;
		}
		cmd_response[cmd_response_len] = '\0';
		//printf("%d: %s\n", i, cmd_response);
		if (cmd_response_check("OK\n")) {
			cmd_receive_state = RECEIVED_OK;
		} else if (cmd_response_check("ERROR")) {
			cmd_receive_state = RECEIVED_ERROR;
			// read out to new line
			while (cmd_response_len < MAX_RESPONSE_LEN - 1 && ((ret = socket_receive((unsigned char *)&cmd_response[cmd_response_len++], 1)) == 1)) {
				if (cmd_response[cmd_response_len - 1] == '\n')
					break;
			}
			cmd_response[cmd_response_len] = '\0';
			if (cmd_response_len == MAX_RESPONSE_LEN - 1) {
				warning("%s: response too long for command, after ERROR detected: %s\n\tresponse so far:\n\t\t%s\n", __PRETTY_FUNCTION__, cmd, cmd_response);
				return -1;
			} else if (ret == -1) {
				warning("%s: incomplete response for command, after ERROR detected: %s\n\tresponse so far:\n\t\t%s\n", __PRETTY_FUNCTION__, cmd, cmd_response);
				return -1;
			}
		}
		//printf("%d: %s\n", i, cmd_response);
		//++i;
	} while (cmd_receive_state == RECEIVING);

	gettimeofday(&end, NULL);
	send_ms = tvdiff_ms(&start, &receive_start);
	receive_ms = tvdiff_ms(&receive_start, &end);
	total_ms = tvdiff_ms(&start, &end);
	printf("response times (ms: send, receive, total): %ld\t%ld\t%ld\n", send_ms, receive_ms, total_ms);

	printf("command response: %s\n", cmd_response);
	return 1;
}
