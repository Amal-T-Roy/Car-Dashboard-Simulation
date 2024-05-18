CC = gcc
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -lcheck -lpthread -lsubunit -lrt -lm

test_my_program: test_Q7.c
	$(CC) $(CFLAGS) -o Q7 test_Q7.c $(LDFLAGS) 

.PHONY: clean
clean:
	rm -f Q7

