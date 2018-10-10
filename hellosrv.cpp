#include "co_routine.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <string>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

static int set_non_block(int sock_fd) {
	int flags;
	flags = fcntl(sock_fd, F_GETFL, 0);
	flags |= O_NONBLOCK;
	flags |= O_NDELAY;
	return fcntl(sock_fd, F_SETFL, flags);
}

struct conn_info {
	int fd;
	bool read_done;
	bool write_done;
};

void del_conn_info(conn_info* info) {
	if (!info) return;
	if (info->read_done && info->write_done) {
		close(info->fd);
		delete info;
	}
}
static void* read_to_null(void* param) {
	co_enable_hook_sys();
	char buf[512];
	conn_info* info = reinterpret_cast<conn_info*>(param);
	for (;;) {
		printf("begin read\n");
		int ret = read(info->fd, buf, sizeof(buf));
		printf("end read\n");
		if (ret == 0) {
			break;
		}else if (ret < 0) {
			if (errno == EAGAIN)
				continue;
			break;
		}
	}
	info->read_done = true;
	del_conn_info(info);
	return 0;
}

static void* tell_remote(void* param) {
	co_enable_hook_sys();
	const char* hello = "hello world";
	conn_info* info = reinterpret_cast<conn_info*>(param);
	printf("begin write\n");
	write(info->fd, hello, strlen(hello) + 1);
	printf("end write\n");
	info->write_done = true;
	del_conn_info(info);
	return 0;
}
int g_listen_fd = 0;
int g_port = 0;
int co_accept(int fd, struct sockaddr *addr, socklen_t *len);
static void* myaccept(void* param) {
	co_enable_hook_sys();

	g_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (g_listen_fd == -1) {
		printf("socket error:%s\n", strerror(errno));
		exit(0);
	}
	struct sockaddr_in srv_addr = { 0 };
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_addr.sin_port = htons(g_port); 
	if (bind(g_listen_fd, (const sockaddr*)&srv_addr, sizeof(srv_addr)) == -1) {
		printf("bind error:%s(port=%d)\n", strerror(errno), g_port);
		exit(-1);
	}
	if (listen(g_listen_fd, 20) == -1) {
		printf("listen error:%s\n", strerror(errno));
		exit(-1);
		return 0;
	}
	for (;;) {
		printf("begin accept\n");
		int fd = co_accept(g_listen_fd, NULL, NULL);
		if (fd < 0) {
			struct pollfd pf = { 0 };
			pf.fd = g_listen_fd;
			pf.events = (POLLIN | POLLERR | POLLHUP);
			co_poll(co_get_epoll_ct(), &pf, 1, 1000);
			continue;
		}
		printf("accepted\n");
		// set_non_block(fd);
		stCoRoutine_t* co = 0;
		conn_info* info = new conn_info;
		info->fd = fd;
		info->read_done = info->write_done = false;
		co_create(&co, 0, read_to_null, info);
		co_resume(co);
		co_create(&co, 0, tell_remote, info);
		co_resume(co);
	}
	return 0;
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Usage:\n"
			"	hellosrv port\n");
		return -1;
	}
	g_port = atoi(argv[1]);
	
	stCoRoutine_t* co = 0;
	co_create(&co, 0, myaccept, 0);
	co_resume(co);
	co_eventloop(co_get_epoll_ct(), 0, 0);
	return 0;
}