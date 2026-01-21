MAKEFLAGS += -r
.PHONY: all clean st mt
.SUFFIXES: .c .o .d
SRC := $(shell find . -type f -name '*.c')
STOBJ := $(patsubst %.c, %.st.o, $(SRC))
MTOBJ := $(patsubst %.c, %.mt.o, $(SRC))
STDEP := $(patsubst %.c, %.st.d, $(SRC))
MTDEP := $(patsubst %.c, %.mt.d, $(SRC))

INCDIR := $(CURDIR)/../include
SRCDIR := $(CURDIR)/../src

CFLAGS ?= -O2 -fno-plt -pipe -flto=auto
CFLAGS += -ggdb3 -fwrapv -fms-extensions -Wall -Wvla -Wno-parentheses -I$(CURDIR) -I$(INCDIR) -I$(SRCDIR)
LDFLAGS ?= -Wl,-O1
LDFLAGS += -L$(SRCDIR)
LDLIBS += -lm -lcaster

all: st mt

st: st.exec
	LD_LIBRARY_PATH=$(SRCDIR) DYLD_LIBRARY_PATH=$(SRCDIR) $(CURDIR)/st.exec

mt: mt.exec
	LD_LIBRARY_PATH=$(SRCDIR) DYLD_LIBRARY_PATH=$(SRCDIR) $(CURDIR)/mt.exec

st.exec: $(STOBJ)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

mt.exec: $(MTOBJ)
	$(LINK.o) $^ $(LOADLIBES) -lpthread $(LDLIBS) -o $@

%.st.o: %.c
	$(COMPILE.c) $< -o $@

%.mt.o: %.c
	$(COMPILE.c) -pthread $< -o $@

clean:
	$(RM) -- $(STOBJ) $(MTOBJ) $(STDEP) $(MTDEP) st.exec mt.exec

-include $(STDEP) $(MTDEP)
%.st.d: %.c cmacs.h
	$(COMPILE.c) -MM $< -o $@

%.mt.d: %.c cmacs.h
	$(COMPILE.c) -pthread -MM $< -o $@

cmacs.h:
	$(RM) -- $@
	cp $(SRCDIR)/$@ $(CURDIR)/$@

.SECONDARY: $(STOBJ) $(MTOBJ)
