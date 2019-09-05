CC      = gcc
CFLAGS  = -Wall -std=c11 -ggdb3
SRCS	= $(wildcard *.c)
OBJS	= $(SRCS:.c=.o)

9cc: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJS): 9cc.h

test: 9cc
	./9cc -test
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean

