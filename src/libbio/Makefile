# The Diu Programming Language.
# Copyright (c) 2024, Diu
# https://furzoom.com
# https://github.com/diu-lang/diu
#
# MIT License

include ../config.mk

LIB = libbio.a

OFILES = \
	binit.$O \

HFIELS = $(DIUROOT)/include/bio.h

install: $(LIB)
	cp $(LIB) $(DIUROOT)/lib

$(LIB): $(OFILES)
	ar rsc $(LIB) $(OFILES)

$(OFILES): $(HFILES)

clean:
	$(RM) -f $(OFILES) $(LIB)

nuke: clean
	$(RM) -f $(DIUROOT)/lib/$(LIB)

.PHONY: all clean nuke
