SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
HDRS := $(wildcard *.h)

tokens: $(OBJS)
	gcc -g -o $@ $(OBJS)

%.o: %.c $(HDRS)
	gcc -c -g -o $@ $<

clean:
	rm -rf tokens $(OBJS)

.PHONY: clean

