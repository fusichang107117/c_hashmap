#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ev.h>
#include "iproxy.h"
#include "iproxylib.h"

int iproxyd_connect(void)
{
	int sockfd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (sockfd < 0) {
		printf("create ipc client error: %d\n", sockfd);
		return -1;
	}

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));

	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, IPC_SOCK_PATH, sizeof(addr.sun_path) - 1);

	int ret = connect(sockfd, (const struct sockaddr *) &addr, sizeof(struct sockaddr_un));
	if (ret == -1) {
		printf("The server is down.\n");
		return -1;
	}
	return sockfd;
}

int iproxy_set(char *key, char *value)
{
	if (!key || *key == '\0') {
		printf("key must not be null\n");
		return -1;
	}

	iproxy_cmd_t cmd;
	char buf[1024] = {'\0'};

	int cmd_len = sizeof(cmd);

	memcpy(cmd.magic,IPROXY,4);
	cmd.id = IPROXY_SET;
	cmd.key_len = strlen(key) + 1;
	cmd.value_len = strlen(value) + 1;
	memcpy(buf,(char *)&cmd, cmd_len);

	//printf("int: %d\n", sizeof(int));
	printf("cmd_len: %d\n", cmd_len);
	printf("key_len: %d\n", cmd.key_len);
	printf("key: %s\n", key);
	printf("value_len: %d\n", cmd.value_len);
	printf("value: %s\n", value);

	if(key){
		snprintf(buf + cmd_len,cmd.key_len,"%s",key);
	}
	if(value) {
		snprintf(buf + cmd_len + cmd.key_len,cmd.value_len,"%s",value);
	}

	int total_len = cmd_len + cmd.key_len + cmd.value_len;

	int sockfd = iproxyd_connect();
	if (sockfd < 0) {
		printf("connect server error\n");
		return -1;
	}
	int len = write(sockfd, buf, total_len);
	close(sockfd);
	printf("send: %d, str_len: %d\n", len, total_len);
	return (len == (total_len + 1)) ? 0 : -1;
}

int iproxy_get(char *key, char *value)
{
	if (!key || *key == '\0' || !value ) {
		printf("key must not be null\n");
		return -1;
	}

	iproxy_cmd_t cmd;
	char buf[1024] = {'\0'};

	int cmd_len = sizeof(cmd);

	memcpy(cmd.magic,IPROXY,4);
	cmd.id = IPROXY_GET;
	cmd.key_len = strlen(key) + 1;
	cmd.value_len = 0;
	memcpy(buf,(char *)&cmd, cmd_len);

	snprintf(buf + cmd_len,cmd.key_len,"%s",key);

	int total_len = cmd_len + cmd.key_len;

	int sockfd = iproxyd_connect();
	if (sockfd < 0) {
		printf("connect server error\n");
		return -1;
	}
	int len = write(sockfd, buf, total_len);

	int ret = read(sockfd, buf, 1024);

	printf("GET: ret: %d, result: %s\n", ret, buf);

	close(sockfd);

	return 0;
}

int iproxy_register(char *key)
{
	return 0;
}

int iproxy_register_and_get(char *key, char *value)
{
	return 0;
}

int iproxy_unregister(char *key)
{
	return 0;
}

int iproxy_commit()
{
	return 0;
}

int main(int argc, char const *argv[])
{
	/* code */
/*	if (argc != 3) {
		printf("use like this: ./iproxylib key value\n");
		return -1;
	}*/
	char buf[1024];
	if(strcmp(argv[1], "set") == 0)
		iproxy_set(argv[2], argv[3]);
	else if(strcmp(argv[1], "get") == 0)
		iproxy_get(argv[2],buf);
	else if(strcmp(argv[1], "register") == 0)
		iproxy_register(argv[2]);
	else if (strcmp(argv[1], "unregister") == 0)
		iproxy_unregister(argv[2]);
	else if (strcmp(argv[1], "commit") == 0)
		iproxy_commit();
	return 0;
}