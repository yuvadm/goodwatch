# GoodWatch Emulator Makefile
# Compiles the emulator using host GCC with firmware source files

CC = gcc
CFLAGS = -Wall -Wno-unused-variable -Wno-unused-function -Wno-builtin-declaration-mismatch -g -O0 -Wno-error
DEFINES = -DEMULATOR_BUILD -Dmain=firmware_main
INCLUDES = -I. -I../firmware -I../firmware/apps

# Firmware source files needed for the emulator
FIRMWARE_SRC = \
	../firmware/main.c \
	../firmware/apps.c \
	../firmware/applist.c \
	../firmware/apps/clock.c \
	../firmware/apps/submenu.c \
	../firmware/apps/settime.c \
	../firmware/bcd.c

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

# Compile emulator sources (without renaming main)
emulator.o: emulator.c
	$(CC) $(CFLAGS) $(INCLUDES) -DEMULATOR_BUILD -Wno-error -c $< -o $@

firmware_stubs.o: firmware_stubs.c
	$(CC) $(CFLAGS) $(INCLUDES) -DEMULATOR_BUILD -Wno-error -c $< -o $@

# Compile firmware sources (with main renamed to firmware_main)
../firmware/%.o: ../firmware/%.c
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
