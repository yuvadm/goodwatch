/*! \file firmware_stubs.c
  \brief Stub implementations of firmware functions for emulator
*/

/* Standard headers first before MSP430 stubs */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>

#include "msp430_stubs.h"

/* Need app struct definition */
#include "../firmware/apps.h"

/* LCD Memory buffers */
uint8_t emu_lcdm[13] = {0};
uint8_t emu_lcdbm[13] = {0};

/* LCD Control registers */
uint8_t emu_lcdbctl0 = 0;
uint8_t emu_lcdbctl1 = 0;
uint8_t emu_lcdbvctl = 0;
uint16_t emu_lcdbpctl0 = 0;
uint16_t emu_lcdbpctl1 = 0;
uint8_t emu_lcdbmemctl = 0;
uint8_t emu_lcdbiv = 0;

/* RTC registers */
uint8_t emu_rtc_hour = 14;
uint8_t emu_rtc_min = 30;
uint8_t emu_rtc_sec = 0;
uint16_t emu_rtc_year = 2025;
uint8_t emu_rtc_mon = 10;
uint8_t emu_rtc_day = 6;
uint8_t emu_rtc_dow = 1;  // Monday
uint8_t emu_rtc_alarm_hour = 0;
uint8_t emu_rtc_alarm_min = 0;

/* Global state */
int has_radio = 0;
int uartactive = 0;

/* Update RTC from system time */
void emu_update_rtc(void) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    emu_rtc_sec = t->tm_sec;
    emu_rtc_min = t->tm_min;
    emu_rtc_hour = t->tm_hour;
    emu_rtc_day = t->tm_mday;
    emu_rtc_mon = t->tm_mon + 1;
    emu_rtc_year = t->tm_year + 1900;
    emu_rtc_dow = t->tm_wday;
}

/* Stub implementations of firmware functions */

void rtc_init(void) {
    emu_update_rtc();
}

void ucs_init(void) {
    /* No-op for emulator */
}

void ref_init(void) {
    /* No-op for emulator */
}

void ref_on(void) {
    /* No-op for emulator */
}

void ref_off(void) {
    /* No-op for emulator */
}

void buzz_init(void) {
    /* No-op for emulator */
}

void codeplug_init(void) {
    /* No-op for emulator */
}

void radio_init(void) {
    has_radio = 0;  // Disable radio in emulator
}

const char* codeplug_name(void) {
    return "EMULATOR";
}

uint32_t codeplug_getfreq(void) {
    return 433920000;
}

void uart_init(void) {
    /* No-op for emulator */
}

void key_init(void) {
    /* No-op for emulator */
}

void sidebutton_init(void) {
    /* No-op for emulator */
}

int sidebutton_mode(void) {
    return 0;  // Not pressed
}

int sidebutton_set(void) {
    return 0;  // Not pressed
}

unsigned int adc_getvcc(void) {
    return 3300;  // 3.3V
}

int power_ishigh(void) {
    return 0;
}

void tone(int note, int duration) {
    /* No-op for emulator */
}

void audio_morse(const char *str, int hold) {
    /* No-op for emulator */
}

/* Dummy POST */
int post(void) {
    return 0;  // Success
}

/* Printf/dmesg stubs */
void dmesg_init(void) {
    /* No-op */
}

void dmesg_putc(void *p, char c) {
    /* Use write() to avoid stdio conflicts */
    write(1, &c, 1);
}


/* Random number stubs */
unsigned int true_rand(void) {
    return (unsigned int)time(NULL);
}

/* BCD conversion functions */
int int2bcd(int num) {
    return ((num / 10) << 4) | (num % 10);
}

unsigned long l2bcd(long num) {
    unsigned long result = 0;
    int shift = 0;

    while (num > 0 && shift < 32) {
        result |= ((unsigned long)(num % 10)) << shift;
        num /= 10;
        shift += 4;
    }

    return result;
}

/* Git hash and ROM save time (dummy values) */
unsigned int GITHASH = 0xDEADBEEF;
unsigned char romsavetime_data[8] = {0, 0, 0, 0, 0xE9, 0x07, 10, 6};  // 2025-10-06
unsigned char *romsavetime = romsavetime_data;

/* Descriptor functions */
void descriptor_dump(void) {
    /* No-op */
}

/* App timer */
void app_cleartimer(void) {
    /* No-op in emulator */
}

/* Minimal app framework stubs - we're skipping the full apps.c */
int appindex = 0;
const struct app *applet = NULL;

void app_keypress(char ch) {
    /* Forward to current app if it has a handler */
    if (applet && applet->keypress) {
        applet->keypress(ch);
    }
}

void app_draw(int forced) {
    if (applet && applet->draw) {
        applet->draw(forced);
    }
}

void app_next(void) {
    /* Stub - don't switch apps in simple emulator */
}

void app_forcehome(void) {
    /* Stub */
}

void app_set(const struct app *newapplet) {
    /* Stub */
}


int submenu_fallthrough(char ch) {
    return 0;
}

void submenu_packettx(void) {
    /* Stub */
}

/* Minimal stub for setting applet */
const struct app setting_applet = {
    .name = "setting",
    .init = NULL,
    .draw = NULL,
    .exit = NULL,
    .keypress = NULL,
    .fallthrough = NULL,
    .packetrx = NULL,
    .packettx = NULL
};

