CC = gcc
CFLAGS = -Werror 

test_my_program: server.c
	$(CC) $(CFLAGS) server.c -o TEST $(LDFLAGS) 

.PHONY: clean
clean:
	rm -f TEST
PID:
	netstat -tulpn | grep ./TEST

