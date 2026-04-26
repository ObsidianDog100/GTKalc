# GTKalc Makefile
# Enhanced GTK4 Calculator
# Author: Harry Elsdon

CC = gcc
CFLAGS = -Wall -Wextra `pkg-config --cflags gtk4 glib-2.0`
LDFLAGS = `pkg-config --libs gtk4 glib-2.0` -lm
OBJECTS = main.o calculator.o ui.o advanced.o history.o memory.o theme.o config.o
TARGET = gtkalc

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# Object files
main.o: main.c calculator.h ui.h
	$(CC) $(CFLAGS) -c main.c

calculator.o: calculator.c calculator.h
	$(CC) $(CFLAGS) -c calculator.c

ui.o: ui.c ui.h calculator.h advanced.h history.h memory.h theme.h config.h
	$(CC) $(CFLAGS) -c ui.c

advanced.o: advanced.c advanced.h
	$(CC) $(CFLAGS) -c advanced.c

history.o: history.c history.h
	$(CC) $(CFLAGS) -c history.c

memory.o: memory.c memory.h
	$(CC) $(CFLAGS) -c memory.c

theme.o: theme.c theme.h
	$(CC) $(CFLAGS) -c theme.c

config.o: config.c config.h
	$(CC) $(CFLAGS) -c config.c

# Clean
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install
install: $(TARGET)
	mkdir -p $(HOME)/.local/bin
	cp $(TARGET) $(HOME)/.local/bin/

# Uninstall
uninstall:
	rm -f $(HOME)/.local/bin/$(TARGET)

# Help
help:
	@echo "GTKalc Build System"
	@echo "==================="
	@echo "make           - Build the calculator"
	@echo "make clean     - Clean build artifacts"
	@echo "make install   - Install to ~/.local/bin"
	@echo "make uninstall - Remove from ~/.local/bin"
	@echo "make help      - Show this help message"

.PHONY: all clean install uninstall help
