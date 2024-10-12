xor_test: xor.c
	gcc -o Xor_test -Wall -Wextra -O3 xor.c -lm

xor_debug: xor.c
	gcc -o Xor_test -Wall -Wextra -g -fsanitize=address  xor.c -lm
