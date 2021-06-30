CFLAGS=-Wall -std=c11 -g
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

scc: $(OBJS)
	cc -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): scc.h

test: scc
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp*