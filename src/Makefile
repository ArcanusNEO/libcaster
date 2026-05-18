MAKEFLAGS += -r
.PHONY: all clean
.SUFFIXES: .c .o .d
SRC := $(shell find . -type f -name '*.c')
OBJ := $(patsubst %.c, %.o, $(SRC))
DEP := $(patsubst %.c, %.d, $(SRC))

WGET := wget -qc --show-progress -t 3 --waitretry=3

CFLAGS ?= -O2 -fno-plt -pipe -flto=auto
CFLAGS += -pthread -fwrapv -fms-extensions -Wall -Wvla -Wno-parentheses -Wno-microsoft -I$(CURDIR) -I$(CURDIR)/extern
LDFLAGS ?= -Wl,-O1
LDLIBS += -lm -lpthread

all: libcaster.a libcaster.so

libcaster.a: $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

libcaster.so: $(OBJ)
	$(LINK.o) -shared $^ $(LOADLIBES) $(LDLIBS) -o $@

%.o: %.c
	$(COMPILE.c) -fPIC $< -o $@

clean:
	$(RM) -- $(OBJ) $(DEP) libcaster.a libcaster.so

-include $(DEP)
%.d: %.c cmacs.h extern
	$(COMPILE.c) -MM $< -o $@

cmacs.h:
	$(WGET) -O $@ https://raw.github.com/ArcanusNEO/cmacs/master/cmacs.h

extern: extern/a5hash.h
extern/a5hash.h:
	-mkdir extern
	$(WGET) -O $@ https://raw.github.com/avaneev/a5hash/main/a5hash.h

.SECONDARY: $(OBJ)
