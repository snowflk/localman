CC = gcc
CFLAGS  = -g -Wall

TARGET = localman
VERSION = 1.0.0
all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

package:
	tar -zcvf localman-$(VERSION).tar.gz --include='*.c' --include='Makefile' .

clean:
	$(RM) $(TARGET)