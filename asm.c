#include <stdio.h>

int main() {
        asm(
                "xor %%ecx,%%ecx\n"
                "loop:\n"
                "pushl %%ecx\n"
                "lea -0x1C(%%ebp),%%eax\n"
		"mov (%%eax),%%edx\n"
		"pushl %%edx\n"
		"lea -0x18(%%ebp), %%ebx\n"
                "call *(%%ebx)\n"
                "inc    %%ecx\n"
                "cmp    $0x3,%%ecx\n"
                "jne loop\n"
                "pushl $0\n"
                "leal -0x28(%%ebp), %%eax\n"
                "pushl %%eax\n"
                "leal -0x38(%%ebp), %%eax\n"
                "pushl %%eax\n"
                "leal -0x14(%%ebp), %%eax\n"
                "call *(%%eax)\n"
		"leave\n"
		"ret\n"

                :::);
return 0;
}
