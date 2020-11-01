export DESTDIR ?=
export PREFIX ?= /usr/local

export INCDIR ?= $(PREFIX)/include/klogging
export LIBDIR ?= $(PREFIX)/lib
export BINDIR ?= $(PREFIX)/bin

MAINVERSION = 0
SUBVERSION = 1
PATCHLEVEL =
VERSION = $(MAINVERSION).$(SUBVERSION)$(if $(PATCHLEVEL),.$(PATCHLEVEL))

OBJECTS = klogging.o
TARGET = libklogging.so

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	g++ -shared $^ -o $@

klogging.o: klogging.cpp klogging.h
	g++ -fPIC -DVERSION=\"$(VERSION)\" -c $<


.PHONY: clean
clean:
	rm -f $(OBJECTS) $(TARGET)


.PHONY: install
install:
	install -d $(DESTDIR)$(INCDIR)/
	install klogging.h $(DESTDIR)$(INCDIR)/
	install -d $(DESTDIR)$(LIBDIR)/
	install $(TARGET) $(DESTDIR)$(LIBDIR)/


.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(INCDIR)/klogging.h
	rm -f $(DESTDIR)$(LIBDIR)/$(TARGET)
