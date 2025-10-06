/*! \file clock_emu.c
  \brief Minimal clock emulator using firmware code

  This integrates with actual firmware files to prove the emulator works.
*/

/* Must include standard libraries FIRST */
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>

/* Now prevent firmware from overriding stdio and printf */
#define _STDIO_H_
#define _PRINTF_H_
#define printf_h_included_
#define stdio_h_included_

/* LCD memory */
uint8_t lcd_mem[13] = {0};

/* lcdm pointer that firmware expects */
volatile unsigned char *lcdm = lcd_mem;
volatile unsigned char *lcdbm = lcd_mem;

/* Import the actual firmware segment map and font */
#include "../firmware/lcdtext.c"





/* Include the real clock app */
#include "clock.h"

/* Undefine firmware's printf redirection */
#ifdef printf
#undef printf
#endif
#ifdef sprintf
#undef sprintf
#endif

/* Declare stdio functions we need */
typedef struct _IO_FILE FILE;
extern int printf(const char *fmt, ...);
extern int fflush(FILE *stream);
extern FILE *stdout;

/* Declare firmware functions we need */
extern void rtc_init(void);
extern void rtc_update(void);

/* Draw the display */
void draw_display(void) {
    /* Update RTC from system time */
    rtc_update();

    /* Draw the clock */
    clock_draw(1);
}

/* Render display */
void render_display(void) {
    int digit_segs[8] = {0};

    /* Decode LCD memory using firmware lcdmap */
    for (int pos = 0; pos < 8; pos++) {
        for (int seg = 0; seg < 7; seg++) {
            int addr = lcdmap[pos][seg];
            int byte = (addr >> 8) & 0xFF;
            int bit = addr & 0xFF;

            if (byte < 13 && (lcd_mem[byte] & bit)) {
                digit_segs[pos] |= (1 << seg);
            }
        }
    }

    printf("\033[2J\033[H");
    printf("╔════════════════════════════════════════════════════╗\n");
    printf("║  GoodWatch Emulator - Using Firmware Code         ║\n");
    printf("╠════════════════════════════════════════════════════╣\n");
    printf("║                                                    ║\n");

    for (int row = 0; row < 5; row++) {
        printf("║    ");
        for (int digit = 7; digit >= 0; digit--) {
            int segs = digit_segs[digit];

            if (row == 0) {
                printf("%s", (segs & 0x01) ? "███" : "   ");
            } else if (row == 1) {
                printf("%s %s", (segs & 0x20) ? "█" : " ", (segs & 0x02) ? "█" : " ");
            } else if (row == 2) {
                printf("%s", (segs & 0x40) ? "███" : "   ");
            } else if (row == 3) {
                printf("%s %s", (segs & 0x10) ? "█" : " ", (segs & 0x04) ? "█" : " ");
            } else {
                printf("%s", (segs & 0x08) ? "███" : "   ");
            }

            printf(" ");

            if (digit == 6 && row == 1) {
                printf("%s", (lcd_mem[3] & 0x20) ? "●" : " ");
            } else if (digit == 6 && row == 3) {
                printf("%s", (lcd_mem[3] & 0x20) ? "●" : " ");
            } else if (digit == 6) {
                printf(" ");
            }
        }
        printf("  ║\n");
    }

    printf("║                                                    ║\n");
    printf("║  %s  %s                                           ║\n",
           (lcd_mem[0] & 0x04) ? "AM" : "  ",
           (lcd_mem[1] & 0x40) ? "PM" : "  ");
    printf("╠════════════════════════════════════════════════════╣\n");
    printf("║  Using firmware: lcdmap, numfont, lcd_digit()     ║\n");
    printf("║  Press Ctrl+C to quit                              ║\n");
    printf("╚════════════════════════════════════════════════════╝\n");
}

int main(void) {
    printf("Starting emulator with firmware integration...\n");
    fflush(stdout);
    sleep(1);

    printf("Calling rtc_init...\n");
    fflush(stdout);
    rtc_init();

    printf("Calling clock_init...\n");
    fflush(stdout);
    clock_init();

    printf("Entering main loop...\n");
    fflush(stdout);

    while (1) {
        draw_display();
        render_display();
        fflush(stdout);
        sleep(1);
    }

    return 0;
}
