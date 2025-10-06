/*! \file msp430.h
  \brief MSP430 hardware stubs for emulator

  This file provides stub definitions for MSP430 hardware to allow
  firmware code to compile and run on a host system.
*/

#ifndef _H_msp430_h
#define _H_msp430_h

/* When building for real MSP430, use the real headers */
#if defined(__GNUC__) && defined(__MSP430__) && !defined(EMULATOR_BUILD)
#include <msp430.h>

/* Otherwise, use our emulator stubs */
#else

#include <stdint.h>

/* Stub out MSP430-specific compiler attributes */
#define __attribute__(x)
#define __interrupt(x)
#define __bis_SR_register(x)
#define __bic_SR_register_on_exit(x)

/* SFR macros for memory-mapped registers */
#define sfrb(name, addr) extern volatile uint8_t name
#define sfrw(name, addr) extern volatile uint16_t name
#define sfr_b(x) extern volatile unsigned char x
#define sfr_w(x) extern volatile unsigned int x
#define sfr_l(x) extern volatile unsigned long x

/* STATUS REGISTER BITS */
#define GIE                    (0x0008)
#define CPUOFF                 (0x0010)
#define OSCOFF                 (0x0020)
#define SCG0                   (0x0040)
#define SCG1                   (0x0080)

#define LPM0_bits              (CPUOFF)
#define LPM1_bits              (SCG0+CPUOFF)
#define LPM2_bits              (SCG1+CPUOFF)
#define LPM3_bits              (SCG1+SCG0+CPUOFF)
#define LPM4_bits              (SCG1+SCG0+OSCOFF+CPUOFF)

/* Bit definitions */
#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

/* RTC Registers */
extern uint8_t RTCHOUR, RTCMIN, RTCSEC, RTCAHOUR, RTCAMIN, RTCDOW;
extern uint16_t RTCYEAR;
extern uint8_t RTCMON, RTCDAY;

/* Watchdog */
sfr_w(WDTCTL);
#define WDTPW  0x5A00
#define WDTHOLD 0x0080

/* LCD_B */
sfr_w(LCDBCTL0);
sfr_w(LCDBMEMCTL);
sfr_w(LCDBVCTL);
sfr_w(LCDBPCTL0);
sfr_w(LCDBPCTL1);

#define LCDON (0x01)
#define LCDSON (0x04)
#define LCDMX0 (0x08)
#define LCDMX1 (0x10)
#define LCDSSEL (0x80)
#define LCDPRE0 (0x0100)
#define LCDPRE1 (0x0200)
#define LCDPRE2 (0x0400)
#define LCDDIV0 (0x0800)
#define LCDDIV1 (0x1000)
#define LCDDIV2 (0x2000)
#define LCDDIV3 (0x4000)
#define LCDDIV4 (0x8000)

#define LCDSTATIC (LCDSON)
#define LCD2MUX (LCDMX0+LCDSON)
#define LCD3MUX (LCDMX1+LCDSON)
#define LCD4MUX (LCDMX1+LCDMX0+LCDSON)

#define LCDCLRM (0x02)
#define VLCD_3_44 (15<<9)
#define LCDCPEN (0x08)

#define LCDM1 (*(volatile unsigned char*)0x0A20)

#endif /* EMULATOR_BUILD */

#endif /* _H_msp430_h */
