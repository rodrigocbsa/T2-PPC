CFLAGS = 
ALL_CFLAGS = -O0 -g $(CFLAGS) -I. -Iinclude -ILibPPC/include

LDFLAGS = 
ALL_LDFLAGS = $(LDFLAGS) -LLibPPC/lib/static/libppc.a

CC=gcc
LD=gcc

# passar como parametro do Makefile o nome do codigo fonte
HEADERS = LibPPC/include/libppc.h
LIBRARIES = LibPPC/lib/static/libppc.a
SRC = matrixmult.c
OBJ = $(SRC:.c=.o)

VPATH = src  

.PHONY: all clean distclean

%.o: %.c $(HEADERS) 
	$(CC) $(ALL_CFLAGS) -c $< -o $@

all: $(OBJ) $(LIBRARIES)
	gcc $(LDFLAGS) $? -o matrixmult
	
LibPPC/lib/static/libppc.a: 
	make -C LibPPC all

clean:
	rm -f *.o src/*.o matrixmult 
	
distclean: clean
	rm -f *.dat
	make -C LibPPC clean


