/*! \file emulator.c
  \brief GoodWatch Firmware Emulator

  Terminal-based emulator for GoodWatch firmware that renders the LCD
  display using ASCII art and accepts keyboard input.
*/

/* Standard C headers first - before firmware headers that override them */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>
#include <time.h>

/* MSP430 stubs must be included before firmware headers */
#include "msp430_stubs.h"

/* Prevent firmware from overriding standard library */
#define printf_h_included_
#define stdio_h_included_

/* Firmware headers - these paths will be adjusted by the Makefile */
#include "../firmware/lcd.h"
#include "../firmware/lcdtext.h"
#include "../firmware/apps.h"

/* Forward declarations */
void render_display(void);
int kbhit(void);
char getch_nonblock(void);
void draw_7segment(int segments, int row_offset);
void setup_terminal(void);
void restore_terminal(void);

static struct termios orig_termios;

/* Setup terminal for non-blocking input */
void setup_terminal(void) {
    struct termios new_termios;

    tcgetattr(STDIN_FILENO, &orig_termios);
    new_termios = orig_termios;

    /* Disable canonical mode and echo */
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 0;
    new_termios.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

/* Restore terminal to original settings */
void restore_terminal(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

/* Check if keyboard input is available */
int kbhit(void) {
    struct timeval tv = {0, 0};
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

/* Get character without blocking */
char getch_nonblock(void) {
    char c = 0;
    if (read(STDIN_FILENO, &c, 1) != 1)
        return 0;
    return c;
}

/* Draw a 7-segment digit in ASCII art */
void draw_7segment(int segments, int row_offset) {
    /* Segment bits: A=1, B=2, C=4, D=8, E=0x10, F=0x20, G=0x40, DP=0x80 */
    const char *lines[5];

    /* Top (A) */
    lines[0] = (segments & 0x01) ? " ███ " : "     ";

    /* Upper middle (F, B) */
    char line1[6];
    snprintf(line1, 6, "%s   %s",
             (segments & 0x20) ? "█" : " ",
             (segments & 0x02) ? "█" : " ");
    lines[1] = line1;

    /* Middle (G) */
    lines[2] = (segments & 0x40) ? " ███ " : "     ";

    /* Lower middle (E, C) */
    char line3[6];
    snprintf(line3, 6, "%s   %s",
             (segments & 0x10) ? "█" : " ",
             (segments & 0x04) ? "█" : " ");
    lines[3] = line3;

    /* Bottom (D) + DP */
    char line4[8];
    snprintf(line4, 8, "%s%s",
             (segments & 0x08) ? " ███ " : "     ",
             (segments & 0x80) ? "." : " ");
    lines[4] = line4;

    printf("%s", lines[row_offset]);
}

/* Render the entire LCD display */
void render_display(void) {
    /* Clear screen and move cursor to top */
    printf("\033[2J\033[H");

    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║           GoodWatch Emulator - Casio CA-53 LCD                ║\n");
    printf("╠════════════════════════════════════════════════════════════════╣\n");

    /* Get segment data for each digit from LCD memory using the lcdmap table */
    extern const int lcdmap[10][8];
    extern const int numfont[];
    extern const int letterfont[];

    int digit_segments[8] = {0};

    /* Decode LCD memory into segment data for each digit position */
    for (int pos = 0; pos < 8; pos++) {
        for (int seg = 0; seg < 8; seg++) {
            int addr = lcdmap[pos][seg];
            int byte = (addr >> 8) & 0xFF;
            int bit = addr & 0xFF;

            if (byte < 13 && (emu_lcdm[byte] & bit)) {
                digit_segments[pos] |= (1 << seg);
            }
        }
    }

    /* Draw 5 rows of the 7-segment display (digits 7-0 left to right) */
    for (int row = 0; row < 5; row++) {
        printf("║  ");
        for (int digit = 7; digit >= 0; digit--) {
            draw_7segment(digit_segments[digit], row);
            printf(" ");

            /* Draw colon between hours and minutes (after digit 6) */
            if (digit == 6 && row == 2) {
                printf("%s", (emu_lcdm[3] & 0x20) ? ":" : " ");
            } else if (digit == 6) {
                printf(" ");
            }
        }
        printf(" ║\n");
    }

    /* Symbol indicators */
    printf("║                                                                ║\n");
    printf("║  %s  %s  %s  %s  %s                                          ║\n",
           (emu_lcdm[0] & 0x04) ? "AM" : "  ",
           (emu_lcdm[1] & 0x40) ? "PM" : "  ",
           (emu_lcdm[7] & 0x40) ? "+" : " ",
           (emu_lcdm[6] & 0x04) ? "-" : " ",
           (emu_lcdm[0xc] & 0x04) ? "÷" : " ");

    printf("╠════════════════════════════════════════════════════════════════╣\n");
    printf("║  Keys: 0-9, +, -, *, /, =, .  |  'q' to quit  |  's' side btn  ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");

    /* Show current app if available */
    if (applet && applet->name) {
        printf("  App: %s\n", applet->name);
    }

    /* Flush output */
}

/* Main emulator loop */
int main(void) {
    char key;
    int running = 1;
    struct timespec last_update = {0, 0};
    struct timespec now;

    printf("Initializing GoodWatch Emulator...\n");

    setup_terminal();

    /* Initialize LCD and clock app */
    lcd_init();
    lcd_zero();

    /* Initialize the application framework */
    extern void clock_init(void);
    extern void clock_draw(int forced);
    extern int clock_keypress(char ch);
    extern void clock_packettx(void);
    extern int clock_exit(void);

    /* Create a clock app structure */
    static const struct app clock_applet = {
        .name = "clock",
        .init = clock_init,
        .draw = clock_draw,
        .exit = clock_exit,
        .keypress = clock_keypress,
        .fallthrough = NULL,
        .packetrx = NULL,
        .packettx = clock_packettx
    };

    /* Set up the clock app */
    applet = &clock_applet;
    clock_init();

    printf("Emulator started. Press 'q' to quit.\n");
    sleep(1);

    /* Main emulation loop - runs at ~4Hz like the real watchdog timer */
    while (running) {
        clock_gettime(CLOCK_MONOTONIC, &now);

        /* Update RTC from system time */
        emu_update_rtc();

        /* Check for keyboard input */
        if (kbhit()) {
            key = getch_nonblock();

            if (key == 'q' || key == 'Q') {
                running = 0;
            } else if (key == 's' || key == 'S') {
                /* Simulate side button - switch apps */
                extern void app_next(void);
                app_next();
            } else if ((key >= '0' && key <= '9') ||
                       key == '+' || key == '-' || key == '*' ||
                       key == '/' || key == '=' || key == '.') {
                /* Pass key to the clock app */
                clock_keypress(key);
            }
        }

        /* Draw the app (like the watchdog timer interrupt) */
        lcd_predraw();
        clock_draw(0);
        lcd_postdraw();

        /* Render to terminal */
        render_display();

        /* Sleep for 250ms (4Hz update rate like the real hardware) */
        usleep(250000);
    }

    restore_terminal();
    printf("\nEmulator stopped.\n");

    return 0;
}
