shell-exploit-32: shell-exploit.c
	gcc -g -fstack-protector-strong -m32 -o shell-exploit-32 shell-exploit.c

test-exploit-32: test-exploit.c
	gcc -g -m32 -DX86 -o test-exploit-32 test-exploit.c

asm-32: asm.c
	gcc -g -m32 -o asm-32 asm.c

clean:
	rm -f shell-exploit-32 test-exploit-32 asm-32
