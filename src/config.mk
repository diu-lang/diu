# The Diu Programming Language.
# Copyright (c) 2024, Diu
# https://furzoom.com
# https://github.com/diu-lang/diu
#
# MIT License

CC = cc
LD = cc
CFLAGS = -ggdb -I$(DIUROOT)/include -O2 -fno-inline
O = o
YFLAGS = -d

# GNU Make syntax
ifndef DIUROOT
DIUBIN = $(HOME)/bin
endif

PWD = $(shell pwd)

%.$O: %.c
	$(CC) $(CFLAGS) -c $(PWD)/$*.c
