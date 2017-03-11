include config.mk

BIN = cuthash
OBJ = $(BIN:=.o)

all: $(BIN)

$(BIN): $(OBJ)

%: %.o
	$(CC) $(LIBS) -o $@ $(LDFLAGS) $<

%.o: %.c config.mk
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) hashit *.o

install: all
	@echo Installing executable file to $(DESTDIR)$(PREFIX)/bin
	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@cp -f $(BIN) $(DESTDIR)$(PREFIX)/bin
	@chmod 755 $(DESTDIR)$(PREFIX)/bin/$(BIN)
	@echo Installing manual page to $(DESTDIR)$(MANPREFIX)/man1
	@mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	@cp -f $(BIN).1 $(DESTDIR)$(MANPREFIX)/man1
	@chmod 644 $(DESTDIR)$(MANPREFIX)/man1/$(BIN).1

.PHONY: all claan install
