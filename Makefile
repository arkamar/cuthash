VERSION = 1.0.1

BIN = cuthash

PREFIX = /usr
MANPREFIX = $(PREFIX)/share/man

CPPFLAGS += -D_DEFAULT_SOURCE
CPPFLAGS += -DOPENSSL_API_COMPAT=0x10100000L

CFLAGS ?= -O2 -pipe
CFLAGS += -Wall -pedantic -Werror=implicit-function-declaration
CFLAGS += $(shell pkg-config --cflags libcrypto)
LDLIBS += $(shell pkg-config --libs libcrypto)

all: $(BIN)

$(BIN).o: $(BIN).c arg.h

clean:
	$(RM) $(BIN) *.o

install: all
	@echo Installing executable file to $(DESTDIR)$(PREFIX)/bin
	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@cp -f $(BIN) $(DESTDIR)$(PREFIX)/bin
	@chmod 755 $(DESTDIR)$(PREFIX)/bin/$(BIN)
	@echo Installing manual page to $(DESTDIR)$(MANPREFIX)/man1
	@mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	@cp -f $(BIN).1 $(DESTDIR)$(MANPREFIX)/man1
	@chmod 644 $(DESTDIR)$(MANPREFIX)/man1/$(BIN).1

.PHONY: all clean install
