#include <stdio.h>

int main() {
        asm(
                "xor %%ecx,%%ecx\n"
                "loop:\n"
                "pushl %%ecx\n"
                "lea -0xC(%%ebp),%%eax\n"
		"mov (%%eax),%%edx\n"
		"pushl %%edx\n"
		"lea -0x8(%%ebp), %%ebx\n"
                "call *(%%ebx)\n"
                "inc    %%ecx\n"
                "cmp    $0x3,%%ecx\n"
                "jne loop\n"
                "pushl $0\n"
                "leal -0x18(%%ebp), %%eax\n"
                "pushl %%eax\n"
                "leal -0x28(%%ebp), %%eax\n"
                "pushl %%eax\n"
                "leal -0x4(%%ebp), %%eax\n"
                "call *(%%eax)\n"
		"leave\n"
		"ret\n"

                :::);
return 0;
}
