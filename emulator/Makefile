# GoodWatch Emulator Makefile
# Compiles the emulator using host GCC with firmware source files

CC = gcc
CFLAGS = -Wall -Wno-unused-variable -Wno-unused-function -Wno-builtin-declaration-mismatch -g -O0
INCLUDES = -I. -I../firmware
DEFINES = -Dmsp430_h_=1 -DEMULATOR_BUILD

# Firmware source files needed for the emulator
FIRMWARE_SRC = \
	../firmware/lcd.c \
	../firmware/lcdtext.c \
	../firmware/bcd.c \
	../firmware/printf.c \
	../firmware/apps/clock.c

# Emulator source files
EMULATOR_SRC = \
	emulator.c \
	firmware_stubs.c

# Combined sources
SOURCES = $(EMULATOR_SRC) $(FIRMWARE_SRC)

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Target binary
TARGET = goodwatch-emu

# Default target
all: $(TARGET)

# Link the emulator
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo ""
	@echo "Build complete! Run with: ./$(TARGET)"
	@echo ""

# Compile emulator sources
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -Wno-error -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f ../firmware/*.o ../firmware/apps/*.o
	@echo "Clean complete"

# Run the emulator
run: $(TARGET)
	./$(TARGET)

# Debug with gdb
debug: $(TARGET)
	gdb ./$(TARGET)

# Show what would be compiled
show:
	@echo "Sources:"
	@echo "$(SOURCES)" | tr ' ' '\n'
	@echo ""
	@echo "Includes: $(INCLUDES)"
	@echo "Defines: $(DEFINES)"

.PHONY: all clean run debug show
