
CFLAGS = 
ALL_CFLAGS = -O0 -g -I.. $(CFLAGS) 

LDFLAGS = 
ALL_LDFLAGS = $(LDFLAGS) ../libppc.a
CC=gcc

# passar como parametro do Makefile o nome do codigo fonte
ALL_TESTS_SRC = $(shell ls *.c)

TESTS = $(ALL_TESTS_SRC:.c=)

%: %.c
	@echo "Compiling: $@" 
	$(CC) $(ALL_CFLAGS) $< -o $@ $(ALL_LDFLAGS)
	chmod +x $@

.PHONY: all clean

all: library $(TESTS)

library: ../libppc.a ../libppc.so
	make -C ../. all

clean-outputs:
	rm -f *.output

clean-inputs:
	rm -f *.input

clean: clean-outputs clean-inputs
	rm -f *.o $(TESTS)