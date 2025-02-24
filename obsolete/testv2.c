#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>

int main()
{
	struct sockaddr_in serv_addr;
	int sock, connfd;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("socket() failed\n");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	const char* ip = "127.0.0.1";
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(5001);
	inet_aton(ip, &serv_addr.sin_addr);

	int ret = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
	if (ret < 0) {
		printf("connect failed\n");
		return -1;
	}

	int flags;
	flags = fcntl(0, F_GETFL, 0);
	fcntl(0, F_SETFL, flags|O_NONBLOCK);
	flags = fcntl(sock,F_GETFL,0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);

	fd_set rfds;
	int retval;
	char buf[100];

	while ( 1 ) {
		memset(buf, 0x0, sizeof(buf));
           
		FD_ZERO(&rfds);
        	FD_SET(0, &rfds);
	   	FD_SET(sock, &rfds);

		retval = select(sock+1, &rfds, NULL, NULL, NULL);

           	if (retval == -1) {
               		perror("select()");
		} else if (retval) {
			int ret;
			if (FD_ISSET(sock, &rfds)) {
				ret = read(sock, buf, sizeof(buf));
				write(1, buf, ret);
			}
			if (FD_ISSET(0, &rfds)) {
				ret = read(0, buf, sizeof(buf));
				if (ret) {
					write(sock, buf, ret);
				}
			}
	   	}
	}

	return 0;
}
