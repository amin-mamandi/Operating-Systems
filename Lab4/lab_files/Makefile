SRCS := $(wildcard *.c)
BINS := $(SRCS:.c=) thread1

all: $(BINS)

# the thread-1.c program must be compiled with the -pthread flag
thread1: thread-1.c
	gcc -o $@ $< -pthread

thread-1: thread-1.c
	gcc -o $@ $< -pthread

# all other .c files can be compiled as normal
%: %.c
	gcc -g $< -o $@

clean:
	rm -rf *.o $(BINS)

.PHONY: clean
