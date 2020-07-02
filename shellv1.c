#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <linux/filter.h>
#include "seccomp-bpf.h"
#include <x86_64-linux-gnu/asm/unistd_32.h>
#include <assert.h>

void install_syscall_filter()
{
        struct sock_filter filter[] = {
                VALIDATE_ARCHITECTURE,
                EXAMINE_SYSCALL,
                ALLOW_SYSCALL(read),
                ALLOW_SYSCALL(write),
                ALLOW_SYSCALL(socket),
                ALLOW_SYSCALL(socketcall),
		ALLOW_SYSCALL(setsockopt),
		ALLOW_SYSCALL(bind),
		ALLOW_SYSCALL(listen),
		ALLOW_SYSCALL(accept4),
                KILL_PROCESS,
        };
        struct sock_fprog prog = {
                .len = (unsigned short)(sizeof(filter)/sizeof(filter[0])),
                .filter = filter,
        };

        assert(prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) == 0);

        assert(prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog) == 0);
}

int sock_read(int connfd) {
	char buf[8];
	int len = 0;
	read(connfd, &len, sizeof(int));
	write(connfd, buf, len);
	len = read(connfd, buf, len);

	return len;
}

int main()
{
	struct sockaddr_in serv_addr;
	int connfd;
	char *bin = "/bin/bash";
	char *argv[3] = { bin, "-i", 0 };
	int enable = 1;

	//install_syscall_filter();

	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) {
		printf("socket() failed\n");
		return -1;
	}
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		printf("setsockopt(SO_REUSEADDR) failed");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5000);

	if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("bind() failed\n");
		return -1;
	}

	
	if (listen(listenfd, 10) < 0) {
		printf("listen() failed\n");
		return -1;
	}
	
	connfd = accept4(listenfd, (struct sockaddr*)NULL, NULL, 0); 
	if (connfd < 0) {
		printf("accept() failed\n");
		return -1;
	}

	sock_read(connfd);

	// shall never return here
	fprintf(stderr, "->>> Return from sock_read!!!\n");
	return -1;
}
