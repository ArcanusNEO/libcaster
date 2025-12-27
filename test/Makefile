MAKEFLAGS += -r
.PHONY: all clean
.SUFFIXES: .c .o .d
SRC := $(shell find . -type f -name '*.c')
OBJ := $(patsubst %.c, %.o, $(SRC))
DEP := $(patsubst %.c, %.d, $(SRC))

INCDIR := $(CURDIR)/../include
SRCDIR := $(CURDIR)/../src

CFLAGS ?= -O2 -fno-plt -pipe -flto=auto
CFLAGS += -ggdb3 -pthread -fwrapv -fms-extensions -Wall -Wvla -Wno-parentheses -Wno-logical-op-parentheses -I$(CURDIR) -I$(INCDIR) -I$(SRCDIR)
LDFLAGS ?= -Wl,-O1
LDFLAGS += -L$(SRCDIR)
LDLIBS += -lm -lpthread -lcaster

all: main
	LD_LIBRARY_PATH=$(SRCDIR) $(CURDIR)/$<

main: $(OBJ)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

%.o: %.c
	$(COMPILE.c) $< -o $@

clean:
	$(RM) -- $(OBJ) $(DEP) main

-include $(DEP)
%.d: %.c cmacs.h
	$(COMPILE.c) -MM $< -o $@

cmacs.h:
	$(RM) -- $@
	cp $(SRCDIR)/$@ $(CURDIR)/$@

.SECONDARY: $(OBJ)
