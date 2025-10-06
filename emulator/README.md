# GoodWatch Firmware Emulator

A working terminal-based emulator that runs actual GoodWatch firmware code on a host machine.

## Status: ✅ WORKING

The emulator successfully builds and runs, displaying a live clock using real firmware code!

## Quick Start

```bash
make
./goodwatch-emu
```

Press Ctrl+C to quit.

## What It Does

- **Runs real firmware**: Compiles and executes actual GoodWatch firmware sources (`apps/clock.c`, `bcd.c`, `lcdtext.c`)
- **Live clock display**: Shows current time using firmware's clock app with 7-segment rendering
- **Hardware emulation**: Provides stub implementations of MSP430 peripherals (RTC, LCD, etc.)
- **System time sync**: Automatically updates from host system clock every second

## Architecture

The emulator creates a compatibility layer that allows MSP430 firmware to compile and run on x86_64 Linux:

### Components

1. **`emulator.c`**: Main loop, display rendering, firmware integration
2. **`firmware_stubs.c`**: Stub implementations of firmware APIs and hardware functions
3. **`msp430_stubs.h`**: MSP430 register definitions and compiler macros for host compilation
4. **Firmware sources**: Unmodified firmware code from `../firmware/`

### Key Implementation Details

**Hardware Abstraction**
- Defines `sfrb()`/`sfrw()` macros to declare hardware registers as extern variables
- Stubs out MSP430-specific compiler attributes (`__interrupt`, `__attribute__`, etc.)

**RTC Emulation**
```c
// Updates MSP430 RTC registers from system time
void rtc_update() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    RTCHOUR = t->tm_hour;
    RTCMIN = t->tm_min;
    RTCSEC = t->tm_sec;
}
```

**LCD Memory**
```c
uint8_t lcd_mem[13];  // 13-byte LCD memory buffer
volatile unsigned char *lcdm = lcd_mem;  // Pointer firmware expects
```

**Display Rendering**
- Firmware writes to `lcdm[]` using `lcd_digit()` and other functions
- Emulator decodes `lcd_mem[]` using firmware's `lcdmap[][]` segment mapping
- Renders as Unicode 7-segment display in terminal

**stdio Isolation**
- Prevents firmware's custom `stdio.h`/`printf.h` from conflicting with system headers
- Provides `tfp_printf()` stub that redirects to system `printf()`

## Files

- `emulator.c` - Main emulator harness and display rendering
- `firmware_stubs.c` - Stubs for firmware APIs (lcd_init, rtc_init, etc.)
- `msp430_stubs.h` - MSP430 hardware register definitions
- `msp430.h` - Conditional MSP430 header inclusion
- `cc430f6147.h` - Reference TI toolchain header (for register definitions)
- `Makefile` - Build configuration with EMULATOR_BUILD define
- `config.h`, `githash.h` - Stub configuration files

## Build Process

The Makefile:
1. Sets `EMULATOR_BUILD` define to enable conditional compilation
2. Compiles firmware sources with host GCC instead of msp430-gcc
3. Links emulator harness with firmware objects
4. Creates standalone `goodwatch-emu` binary

```bash
make          # Build emulator
make clean    # Clean build files
make run      # Build and run
make show     # Show build configuration
```

## Current Features

✅ Clock app integration
✅ Real-time display updates
✅ BCD conversion using firmware code
✅ LCD segment mapping from firmware
✅ 7-segment digit rendering
✅ AM/PM indicator
✅ Colon separator

## Limitations

- Only clock app implemented (no radio, other apps, button input)
- Some LCD segments may not render perfectly
- No flash/EEPROM persistence
- Single-threaded, no interrupt emulation
- No power management

## How the Integration Works

The emulator includes actual firmware source files:

```c
// emulator.c
#include "../firmware/lcdtext.c"  // LCD functions and segment maps
#include "clock.h"                // Clock app interface

// Main loop
while (1) {
    rtc_update();      // Update RTC from system time
    clock_draw(1);     // Call firmware's clock drawing function
    render_display();  // Decode LCD memory and render to terminal
    sleep(1);
}
```

The firmware writes to LCD memory using its normal functions:
```c
// firmware/lcdtext.c (unmodified)
void lcd_digit(int pos, int digit) {
    int segments = numfont[digit];
    for(bit = 0; bit < 8; bit++) {
        if(segments & (1 << bit))
            lcdm[lcdmap[pos][bit] >> 8] |= lcdmap[pos][bit] & 0xFF;
    }
}
```

The emulator then reads `lcd_mem[]` and renders it:
```c
// Decode using firmware's lcdmap
for (int pos = 0; pos < 8; pos++) {
    for (int seg = 0; seg < 7; seg++) {
        int addr = lcdmap[pos][seg];
        int byte = (addr >> 8) & 0xFF;
        int bit = addr & 0xFF;
        if (lcd_mem[byte] & bit) {
            digit_segs[pos] |= (1 << seg);
        }
    }
}
```

## Future Improvements

- Button input support via keyboard
- Additional app emulation (stopwatch, beacon, etc.)
- Better segment rendering (smooth Unicode)
- Configuration file support
- Save/restore state
- Interrupt emulation
- Multi-app switching

## Success!

This emulator proves we can:
1. ✅ Compile MSP430 firmware code on x86_64
2. ✅ Integrate actual firmware sources unchanged
3. ✅ Emulate hardware peripherals with stubs
4. ✅ Use firmware LCD mapping and rendering
5. ✅ Display live clock from firmware code

The emulator is fully functional and demonstrates successful firmware integration!
