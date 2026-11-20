CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -Wpedantic
PREFIX ?= /usr/local

TARGET := launcher

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $<

test: $(TARGET)
	sh tests/test.sh ./$(TARGET)

install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 0755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)

clean:
	$(RM) $(TARGET) $(TARGET).exe

.PHONY: all test install clean
