#include <stdio.h>

int main() {
        asm(
		"pop %%rsi\n"
		"ret\n"

                :::);
return 0;
}
