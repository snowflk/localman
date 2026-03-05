CC ?= gcc
CFLAGS ?= -g -Wall
LDFLAGS ?=
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
DESTDIR ?=

TARGET := localman
VERSION ?= $(shell cat VERSION 2>/dev/null || echo 0.0.0)
DIST_NAME := $(TARGET)-$(VERSION)
DIST_ARCHIVE := $(DIST_NAME).tar.gz

.PHONY: all build clean install uninstall dist distclean package

all: build

build: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(TARGET).c

install: $(TARGET)
	install -d $(DESTDIR)$(BINDIR)
	install -m 0755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)

uninstall:
	$(RM) $(DESTDIR)$(BINDIR)/$(TARGET)

dist: distclean
	mkdir -p $(DIST_NAME)
	cp -f README.md Makefile localman.c $(DIST_NAME)/
	tar -czf $(DIST_ARCHIVE) $(DIST_NAME)
	$(RM) -r $(DIST_NAME)

package: dist

clean:
	$(RM) $(TARGET)

distclean: clean
	$(RM) $(DIST_ARCHIVE)
