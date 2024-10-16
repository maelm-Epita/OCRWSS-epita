xor_test: xor.c
	gcc -o Xor_test -Wall -Wextra -O3 xor.c -lm

xor_debug: xor.c
	gcc -o Xor_test -Wall -Wextra -g -fsanitize=address  xor.c -lm

test:	test.c Network.c math_helpers.c arr_helpers.c
	gcc -o test -Wall -Wextra -g -fsanitize=address test.c Network.c math_helpers.c arr_helpers.c -lm

