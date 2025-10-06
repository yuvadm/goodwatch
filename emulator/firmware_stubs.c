

/*! \file firmware_stubs.c
  \brief Stub implementations of firmware functions for emulator
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>

/* Forward declare stdio functions we need */
typedef struct _IO_FILE FILE;
extern int vprintf(const char *fmt, va_list ap);
extern int vsprintf(char *str, const char *fmt, va_list ap);
extern int putchar(int c);
extern int fflush(FILE *stream);
extern FILE *stdout;

/* Prevent firmware from including its stdio */
#define _STDIO_H_
#define _PRINTF_H_
#define printf_h_included_
#define stdio_h_included_

#include "apps.h"

/* Undefine printf macro from firmware so we can define the real function */
#ifdef printf
#undef printf
#endif
#ifdef sprintf
#undef sprintf
#endif

/* RTC registers */
uint8_t RTCHOUR, RTCMIN, RTCSEC, RTCAHOUR, RTCAMIN, RTCDOW;
uint16_t RTCYEAR;
uint8_t RTCMON, RTCDAY;
unsigned char *romsavetime;

/* LCD Control registers */
uint8_t emu_lcdbmemctl;
uint8_t emu_lcdbctl0;
uint8_t emu_lcdbvctl;
uint16_t emu_lcdbpctl0;
uint16_t emu_lcdbpctl1;

/* LCD Memory */
extern uint8_t lcd_mem[13];

void rtc_init() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    RTCHOUR = t->tm_hour;
    RTCMIN = t->tm_min;
    RTCSEC = t->tm_sec;
    RTCDAY = t->tm_mday;
    RTCMON = t->tm_mon + 1;
    RTCYEAR = t->tm_year + 1900;
    RTCDOW = t->tm_wday;
}

void rtc_update() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    RTCHOUR = t->tm_hour;
    RTCMIN = t->tm_min;
    RTCSEC = t->tm_sec;
}

/* BCD conversion functions */
int int2bcd(int num) {
    return ((num / 10) << 4) | (num % 10);
}

long l2bcd(long num) {
    long result = 0;
    int shift = 0;
    while (num > 0 && shift < 32) {
        result |= ((long)(num % 10)) << shift;
        num /= 10;
        shift += 4;
    }
    return result;
}



/* Hardware register stubs */
unsigned int WDTCTL;
unsigned int LCDBCTL0, LCDBMEMCTL, LCDBVCTL, LCDBPCTL0, LCDBPCTL1;
unsigned int LCDBCTL1, LCDBIV;
unsigned int UCSCTL7, RF1AIFERR;
unsigned int PJDIR, PJOUT;
unsigned int SFRIE1, WDTIE;
unsigned int PMMCTL0;
int uartactive = 0;

/* Hardware initialization stubs */
void dmesg_init(void) {}
void ref_init(void) {}
void lcd_init(void) {}
void lcd_zero(void) {}
void lcd_predraw(void) {}
void lcd_postdraw(void) {}
void ucs_init(void) {}
void buzz_init(void) {}
void codeplug_init(void) {}
void radio_init(void) {}
void key_init(void) {}
void sidebutton_init(void) {}
void uart_init(void) {}

/* RNG */
unsigned int true_rand(void) { return 12345; }
void srand(unsigned int seed) {}

/* dmesg */
void dmesg_putc(void *p, char c) {
    putchar(c);
}

void init_printf(void *putp, void (*putf)(void*, char)) {
    /* We'll just use real printf */
}

/* RTC helpers */
void SetRTCDOW(int dow) {}
int GetRTCYEAR() { return 2025; }

/* Audio */
void audio_morse(const char *str, int hold) {}

/* Side button */
int sidebutton_set(void) { return 0; }
int sidebutton_mode(void) { return 0; }

/* RTC Setters - settime app uses these */
void SetRTCSEC(int sec) { RTCSEC = sec; }
void SetRTCMIN(int min) { RTCMIN = min; }
void SetRTCHOUR(int hour) { RTCHOUR = hour; }
void SetRTCDAY(int day) { RTCDAY = day; }
void SetRTCMON(int mon) { RTCMON = mon; }
void SetRTCYEAR(int year) { RTCYEAR = year; }
void rtc_setdow(void) {
    /* Calculate day of week from date - stub for now */
    RTCDOW = 0;
}

/* Power management */
void ref_on(void) {}
void ref_off(void) {}
unsigned int adc_getvcc(void) { return 3300; }
int power_ishigh(void) { return 0; }

/* Radio */
int has_radio = 0;
const char* codeplug_name(void) { return "EMULATOR"; }
long codeplug_getfreq(void) { return 433920000; }
void codeplug_setfreq(void) {}
void radio_on(void) {}
void radio_writesettings(int x) {}
void radio_writepower(int x) {}
void radio_setfreq(long freq) {}
void radio_strobe(int x) {}
void radio_morse(const char *str) {}
#define RF_SCAL 0

/* Submenu - now included from submenu.c, don't duplicate */

/* Monitor */
void monitor_packetrx(uint8_t *packet, int len) {}

/* Descriptor */
void descriptor_dump(void) {}

/* App management - setting_applet is defined in applist.c */

/* Display mode */
int flickermode = 0;

/* Printf stubs */
void tfp_printf(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void tfp_sprintf(char *str, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsprintf(str, fmt, args);
    va_end(args);
}

/* Provide real printf implementation */
int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vprintf(fmt, args);
    va_end(args);
    return ret;
}

/* Emulator main loop - replaces the firmware's interrupt-driven model */
extern int firmware_main(void); /* Forward declare firmware's main (renamed) */
extern void emulator_render_display(void);
extern const struct app *applet;

#include <setjmp.h>
static jmp_buf emulator_jmp;

/* Called by __bis_SR_register macro to exit firmware_main and return here */
void emulator_exit_to_main(void) {
    longjmp(emulator_jmp, 1);
}

int emulator_main(void) {
    /* Set up jump point for when firmware enters low power mode */
    if (setjmp(emulator_jmp) == 0) {
        /* First time - call firmware's main - it will initialize everything */
        firmware_main();
        /* Should never get here - firmware_main should call __bis_SR_register */
        printf("Warning: firmware_main returned without entering LPM!\n");
    }

    /* We jumped back from __bis_SR_register - firmware is initialized */
    printf("Firmware initialized, entering emulation loop...\n");

    while (1) {
        /* Update RTC from system time */
        rtc_update();

        /* Simulate watchdog timer - call app draw */
        if (applet && applet->draw) {
            lcd_predraw();
            applet->draw(0);  /* Not forced */
            lcd_postdraw();
        }

        /* Render to terminal */
        emulator_render_display();
        fflush(stdout);

        /* Sleep 1 second (firmware updates 4x/sec but we'll do 1x/sec) */
        sleep(1);
    }

    return 0;
}
