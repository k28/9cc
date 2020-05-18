CC      = gcc
CFLAGS  = -Wall -std=c11 -ggdb3
SRCS	= $(wildcard *.c)
OBJS	= $(SRCS:.c=.o)

9cc: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJS): 9cc.h

test: 9cc test/test.c
	./9cc -test
	#./test.sh
	@./9cc -f test/test.c > tmp.s
	@gcc -static -o tmp tmp.s test-tmp.o
	@./tmp

run: 9cc
	./run.sh

clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean

