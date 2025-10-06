

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
extern int vprintf(const char *fmt, va_list ap);
extern int vsprintf(char *str, const char *fmt, va_list ap);

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



void lcd_init() {}
void lcd_zero() {}
void SetRTCDOW(int dow) {}
int GetRTCYEAR() { return 2025; }
void sidebutton_init() {}
void audio_morse(const char *str, int hold) {}
int sidebutton_set(void) { return 0; }
const struct app *setting_applet;
void app_set(const struct app *newapplet) {}
int post(void) { return 0; }
void ref_on(void) {}
unsigned int adc_getvcc(void) { return 3300; }
void ref_off(void) {}
int has_radio = 0;
const char* codeplug_name(void) { return "EMULATOR"; }
long codeplug_getfreq(void) { return 433920000; }
int submenu_fallthrough(char ch) { return 0; }
void submenu_packettx(void) {}
void ucs_init(void) {}
int power_ishigh(void) { return 0; }
void tfp_sprintf(char *str, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsprintf(str, fmt, args);
    va_end(args);
}
int flickermode = 0;

/* Provide real printf implementation for firmware that calls tfp_printf */
int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vprintf(fmt, args);
    va_end(args);
    return ret;
}
