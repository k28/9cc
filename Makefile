
CC      = gcc
CFLAGS  = -ggdb3

9cc: 9cc.c
	$(CC) $(CFLAGS) -o $@ $^

test: 9cc
	./9cc -test
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp*

