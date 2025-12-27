MAKEFLAGS += -r
.PHONY: all clean install uninstall test

prefix ?= /usr/local
exec_prefix := $(prefix)
bindir := $(exec_prefix)/bin
sbindir := $(exec_prefix)/sbin
libexecdir := $(exec_prefix)/libexec
datarootdir := $(prefix)/share
datadir := $(datarootdir)
sysconfdir := $(prefix)/etc
sharedstatedir := $(prefix)/com
localstatedir := $(prefix)/var
runstatedir := $(localstatedir)/run
includedir := $(prefix)/include
docdir := $(datarootdir)/doc
infodir := $(datarootdir)/info
htmldir := $(docdir)
dvidir := $(docdir)
pdfdir := $(docdir)
psdir := $(docdir)
libdir := $(exec_prefix)/lib
lispdir := $(datarootdir)/emacs/site-lisp
localedir := $(datarootdir)/locale
mandir := $(datarootdir)/man
manext := .1

all:
	$(MAKE) -C src -- all

clean:
	$(MAKE) -C src -- clean
	$(RM) -- src/cmacs.h

install: all
	mkdir -p -- $(DESTDIR)$(includedir)
	install -m644 -- include/grimoire.h $(DESTDIR)$(includedir)

	mkdir -p -- $(DESTDIR)$(libdir)
	install -m755 -- src/libcaster.a $(DESTDIR)$(libdir)
	install -m755 -- src/libcaster.so $(DESTDIR)$(libdir)

uninstall:
	$(RM) -- $(DESTDIR)$(includedir)/grimoire.h
	$(RM) -- $(DESTDIR)$(libdir)/libcaster.a
	$(RM) -- $(DESTDIR)$(libdir)/libcaster.so

test:
	$(MAKE) -C test -- all
