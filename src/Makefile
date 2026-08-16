MAKEFLAGS += -r
.PHONY: all clean
.SUFFIXES: .c .o .d
SRC := $(shell find . -path ./src -prune -o -type f -name "*.c" -print)
OBJ := $(patsubst %.c, %.o, $(SRC))
DEP := $(patsubst %.c, %.d, $(SRC))

EXT := cmacs.h
WGET := wget -qc --show-progress -t 3 --waitretry=3

CFLAGS ?= -O3 -fno-plt -pipe
CFLAGS += -D_GNU_SOURCE=1 -pthread -D_REENTRANT -fwrapv -fms-extensions -Wall -Wvla -Wno-parentheses -Wno-microsoft -I$(CURDIR) -I$(CURDIR)/extern -I$(CURDIR)/include
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
%.d: %.c $(EXT)
	$(COMPILE.c) -MM $< -o $@

extern: $(EXT)
	@install -d $@

cmacs.h:
	@$(WGET) -O $@ https://raw.github.com/ArcanusNEO/cmacs/master/cmacs.h || ($(RM) $@ && false)

.SECONDARY: $(OBJ)
