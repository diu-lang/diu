# The Diu Programming Language.
# Copyright (c) 2024, Diu
# https://furzoom.com
# https://github.com/diu-lang/diu
#
# MIT License

CFLAGS ?= -Wall -Werror -Wextra
LDFLAGS ?=

BISON ?= bison
BISON_FLAGS = --defines

all: diu

diu: lex.o diu.yy.o
	$(CC) $(CFLAGS) -o $@ $^

lex.o: lex.c diu.h diu.yy.h

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

diu.yy.c: diu.y
	$(BISON) $(BISON_FLAGS) -o $@ $<

diu.yy.h: diu.yy.c

# Override builtin rules
%.c: %.y

.PHONY: all clean

clean:
	$(RM) -r *.o *.yy.[ch] diu

