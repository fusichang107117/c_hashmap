/*
 * A unit test and example of how to use the simple C hashmap
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ev.h>

//#include "hashmap.h"

#define IPC_SOCK_PATH 	"/tmp/mirror_ipc.socket"
#define MAX_IPC_CLIENT_FDS	50
#define MAX_BUF_SIZE 1024

struct ev_async async;

int ipc_client_count = 0;

static int ipc_server_init(void)
{
	int fd;
	struct sockaddr_un addr;

	unlink(IPC_SOCK_PATH);

	fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (fd < 0) {
		printf("create ipc server error: %d\n", fd);
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, IPC_SOCK_PATH, sizeof(addr.sun_path) - 1);

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		printf("bind ipc server error\n");
		return -1;
	}

	if (listen(fd, MAX_IPC_CLIENT_FDS) < 0) {
		printf("listen ipc server error\n");
		return -1;
	}

	return fd;
}

static void ipc_recv_handle(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
	char buf[MAX_BUF_SIZE] = { 0 };

	int ret = read(watcher->fd, buf, MAX_BUF_SIZE);
	if (ret <= 0) {
		perror("read");
		close(watcher->fd);
		return;
	}
	buf[ret] = '\0';
	printf("ret:%d, %s\n", ret, buf);
}

static void ipc_accept_handle(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
	int client_fd;

	if (EV_ERROR & revents) {
		printf("error event in accept\n");
		return;
	}
	if (ipc_client_count >= MAX_IPC_CLIENT_FDS) {
		printf("too many ipc client to track %d.\n", ipc_client_count);
		return;
	}

	client_fd = accept(watcher->fd, NULL, NULL);
	if (client_fd < 0) {
		printf("accept error\n");
		return;
	}

	struct ev_io *w_client = (struct ev_io *)malloc(sizeof(struct ev_io));
	if (!w_client) {
		printf("%s(), %d: malloc error\n", __func__, __LINE__);
		close(client_fd);
		return;
	}

	ipc_client_count++;
	printf("client %d connected,total %d clients.\n", client_fd, ipc_client_count);

	ev_io_init(w_client, ipc_recv_handle, client_fd, EV_READ);
	ev_io_start(loop, w_client);
}

void sig_stop_ev(void)
{
	ev_async_send(EV_DEFAULT_ &async);
	ev_break(EV_DEFAULT_ EVBREAK_ALL);
}

int main(int argc, char const *argv[])
{
	struct ev_loop *loop = EV_DEFAULT;

	struct ev_io ipc_server;
	int ipc_serverfd;
	ipc_serverfd = ipc_server_init();
	if (ipc_serverfd < 0) {
		return -1;
	}

	ev_io_init(&ipc_server, ipc_accept_handle, ipc_serverfd, EV_READ);
	ev_io_start(loop, &ipc_server);

	ev_async_init(&async, sig_stop_ev);
	ev_async_start(loop, &async);

	ev_run(loop, 0);

	ev_default_destroy();

	if (ipc_serverfd > 0)
		close(ipc_serverfd);

	unlink(IPC_SOCK_PATH);

	return 0;
}