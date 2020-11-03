export DESTDIR ?=
export PREFIX ?= /usr/local

export INCDIR ?= $(PREFIX)/include/etinyalsa
export LIBDIR ?= $(PREFIX)/lib
export BINDIR ?= $(PREFIX)/bin

MAINVERSION = 0
SUBVERSION = 1
PATCHLEVEL =
VERSION = $(MAINVERSION).$(SUBVERSION)$(if $(PATCHLEVEL),.$(PATCHLEVEL))

HEADER = easoundlib.h
LIB_OBJECTS = epcm.o
LIB_TARGET = libetinyalsa.so
BIN_OBJECTS = ecap.o
BIN_TARGET = ecap

.PHONY: all
all: $(LIB_TARGET) $(BIN_TARGET)

$(LIB_TARGET): $(LIB_OBJECTS)
	gcc -shared $^ -o $@

epcm.o: epcm.c $(HEADER)
	gcc -fPIC -O2 -DVERSION=\"$(VERSION)\" -c $<

$(BIN_TARGET): $(BIN_OBJECTS) $(LIB_TARGET)
	gcc $^ -lklogging -lpthread -ltinyalsa -ldl $(LIB_TARGET) -o $@

ecap.o: ecap.c
	gcc -fPIC -DVERSION=\"$(VERSION)\" -c $<



.PHONY: clean
clean:
	rm -f $(LIB_OBJECTS) $(LIB_TARGET)
	rm -f $(BIN_OBJECTS) $(BIN_TARGET)


.PHONY: install
install:
	install -d $(DESTDIR)$(INCDIR)/
	install $(HEADER) $(DESTDIR)$(INCDIR)/
	install -d $(DESTDIR)$(LIBDIR)/
	install $(LIB_TARGET) $(DESTDIR)$(LIBDIR)/
	install -d $(DESTDIR)$(BINDIR)/
	install $(BIN_TARGET) $(DESTDIR)$(BINDIR)/

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(INCDIR)/$(HEADER)
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_TARGET)
	rm -f $(DESTDIR)$(BINDIR)/$(BIN_TARGET)
