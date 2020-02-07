SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
HDRS := $(wildcard *.h)
PLS := $(wildcard *.plist)

tokens: $(OBJS)
	gcc -g -o $@ $(OBJS)

%.o: %.c $(HDRS)
	gcc -c -g -o $@ $<

clean:
	rm -rf tokens $(OBJS) $(PLS)

.PHONY: clean

