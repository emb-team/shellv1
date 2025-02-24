#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>

void func() {
	printf("success!\n");
	exit(0);
}

int main()
{
	long *ptr;
	void (*f)();
	ptr = (long *)&ptr;
	ptr = (long*) ((long)ptr & (~0xfff));
	printf("%x\n", ptr);
	int a = mprotect(ptr, 0x1000, 0x7);
	if (a == 0) {
		printf("success. Stack area remapped!\n");
		return -1;
	}
	memcpy(ptr, func, 0x100);
	f = (void *)ptr;
	printf("f = %x func() = %x\n", f, func);
	f();
	return 0;
}
