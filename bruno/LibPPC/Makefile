CFLAGS = 
ALL_CFLAGS = -O0 -g $(CFLAGS)

LDFLAGS = 
ALL_LDFLAGS = $(LDFLAGS)

CC=gcc
LD=gcc

# passar como parametro do Makefile o nome do codigo fonte
HEADERS = include/libppc.h
SRC = libpcc.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean clean-obj static shared prepare

VPATH = src

%.o: %.c $(HEADERS)
	$(CC) -I. -Iinclude $(ALL_CFLAGS) -c $< -o $@

prepare:
	mkdir -p obj/shared
	mkdir -p obj/static
	mkdir -p lib/shared
	mkdir -p lib/static

all:
	make prepare
	make CFLAGS="$(CFLAGS)" static
	make clean-obj
	make CFLAGS="$(CFLAGS) -fPIC" shared

static: $(OBJ)
	$(AR) cru lib/static/libppc.a $<

shared: $(OBJ)
	$(LD) -shared $(ALL_LDFLAGS) $< -o lib/shared/libppc.so

clean-obj:
	rm -rf *.o

clean-shared:
	rm -rf lib/shared/*

clean-static:
	rm -rf lib/static/*

clean: clean-obj clean-static clean-shared

install: all
	


