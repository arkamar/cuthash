VERSION = 0.0

# Customize below to fit your system

# paths
PREFIX = /usr
MANPREFIX = $(PREFIX)/share/man

CFLAGS += -D_DEFAULT_SOURCE
CFLAGS += `pkg-config --cflags libcrypto`
LIBS = `pkg-config --libs libcrypto`
