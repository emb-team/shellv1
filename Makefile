shell-exploit: shell-exploit.c
	gcc -fstack-protector-strong -m32 -o shell-exploit shell-exploit.c

test-exploit: test-exploit.c
	gcc -g -m32 -o test-exploit test-exploit.c

asm: asm.c
	gcc -g -m32 -o asm asm.c

clean:
	rm -f shell-exploit test-exploit asm
