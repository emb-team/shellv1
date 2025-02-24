all: shell-exploit-64-optimized test-exploit-64-optimized shell-exploit-64 shell-exploit-32 test-exploit-32 test-exploit-64 asm-32 asm-64

shell-exploit-64-optimized: shell-exploit.c
	gcc -O3 -DX86_64 -fstack-protector-strong -o shell-exploit-64-optimized shell-exploit.c

shell-exploit-64: shell-exploit.c
	gcc -DX86_64 -fstack-protector-strong -o shell-exploit-64 shell-exploit.c

shell-exploit-32: shell-exploit.c
	gcc -DX86 -fstack-protector-strong -m32 -o shell-exploit-32 shell-exploit.c

test-exploit-64-optimized: test-exploit.c
	gcc -g -DX86_64 -DOPTIMIZE -o test-exploit-64-optimized test-exploit.c

test-exploit-64: test-exploit.c
	gcc -g -DX86_64 -o test-exploit-64 test-exploit.c

test-exploit-32: test-exploit.c
	gcc -g -m32 -DX86 -o test-exploit-32 test-exploit.c

asm-32: asm_x32.c
	gcc -g -m32 -o asm-32 asm_x32.c

asm-64: asm_x64.c
	gcc -g -o asm-64 asm_x64.c
clean:
	rm -f shell-exploit-* test-exploit-* asm-*
