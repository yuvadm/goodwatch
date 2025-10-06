# GoodWatch Firmware Emulator

A terminal-based emulator for the GoodWatch firmware that runs on your host system.

## Overview

This emulator allows you to test and develop GoodWatch firmware without physical hardware. It renders the 7-segment LCD display as ASCII art and accepts keyboard input to simulate button presses.

## Architecture

- **msp430_stubs.h** - MSP430 hardware register stubs
- **firmware_stubs.c** - Stub implementations of firmware APIs
- **emulator.c** - Main emulator with terminal display and input handling
- **Makefile** - Builds emulator with actual firmware sources

## Building

```bash
cd emulator
make
```

## Running

```bash
./goodwatch-emu
```

Or:

```bash
make run
```

## Controls

- **0-9** - Number buttons
- **+, -, \*, /, =, .** - Symbol buttons
- **s** - Side button (switch apps)
- **q** - Quit emulator

## How It Works

The emulator:

1. **Stubs MSP430 hardware** - Replaces hardware registers with emulated variables
2. **Reuses firmware code** - Compiles actual firmware .c files (lcd.c, lcdtext.c, clock.c)
3. **Emulates LCD memory** - 13-byte buffer decoded using firmware's lcdmap table
4. **Renders to terminal** - Converts 7-segment data to ASCII art
5. **Simulates timing** - Runs at 4Hz like the real watchdog timer interrupt

## Integration with Firmware

The emulator directly includes firmware sources:

```
../firmware/lcd.c        -> LCD memory management
../firmware/lcdtext.c    -> 7-segment font rendering
../firmware/apps.c       -> App framework
../firmware/apps/clock.c -> Clock app (default)
```

The MSP430-specific code is replaced at compile time via `msp430_stubs.h` which defines:
- RTC registers backed by system time
- LCD memory buffers (emu_lcdm[13], emu_lcdbm[13])
- Control registers and interrupts as no-ops

## Adding More Apps

To test other apps, modify `emulator.c`:

```c
extern void myapp_init(void);
extern void myapp_draw(int forced);

// In main():
myapp_init();

// In loop:
myapp_draw(0);
```

Then add the app source to `FIRMWARE_SRC` in the Makefile.

## Debugging

```bash
make debug
```

This opens gdb with the emulator, allowing you to step through firmware code.

## Limitations

- No radio emulation
- No buzzer/audio
- No ADC readings (returns fixed 3.3V)
- Simplified RTC (uses system time)
- Single app at a time (no full app switching yet)

## Future Enhancements

- Full app switching support
- SDL/GUI rendering
- Radio packet simulation
- Configuration file for initial state
- Recording/playback of button sequences
