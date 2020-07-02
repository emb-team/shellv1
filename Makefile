shellv1: shellv1.c
	gcc -fstack-protector-strong -m32 -o shellv1 shellv1.c

testv1: testv1.c
	gcc -g -m32 -o testv1 testv1.c

asm: asm.c
	gcc -g -m32 -o asm asm.c

clean:
	rm -f shellv1 testv1 asm
