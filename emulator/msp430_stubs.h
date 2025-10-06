/*! \file msp430_stubs.h
  \brief MSP430 hardware stubs for emulator

  This file provides stub definitions for MSP430 hardware registers
  to allow firmware code to compile on a host system.
*/

#ifndef MSP430_STUBS_H
#define MSP430_STUBS_H

#include <stdint.h>
#include <time.h>

/* Stub out MSP430-specific compiler attributes */
#define __attribute__(x)
#define __interrupt(x)

/* RTC Registers - backed by host system time */
extern uint8_t emu_rtc_hour;
extern uint8_t emu_rtc_min;
extern uint8_t emu_rtc_sec;
extern uint16_t emu_rtc_year;
extern uint8_t emu_rtc_mon;
extern uint8_t emu_rtc_day;
extern uint8_t emu_rtc_dow;
extern uint8_t emu_rtc_alarm_hour;
extern uint8_t emu_rtc_alarm_min;

#define RTCHOUR emu_rtc_hour
#define RTCMIN  emu_rtc_min
#define RTCSEC  emu_rtc_sec
#define RTCYEAR emu_rtc_year
#define RTCMON  emu_rtc_mon
#define RTCDAY  emu_rtc_day
#define RTCDOW  emu_rtc_dow
#define RTCAHOUR emu_rtc_alarm_hour
#define RTCAMIN  emu_rtc_alarm_min
#define RTCAE    0x80

/* Port definitions (unused in emulator) */
#define P1SEL (*(volatile uint8_t*)0)
#define P1DIR (*(volatile uint8_t*)0)
#define P1REN (*(volatile uint8_t*)0)
#define P1OUT (*(volatile uint8_t*)0)
#define P1IN  (*(volatile uint8_t*)0)
#define P2SEL (*(volatile uint8_t*)0)
#define P2DIR (*(volatile uint8_t*)0)
#define P2REN (*(volatile uint8_t*)0)
#define P2OUT (*(volatile uint8_t*)0)
#define P2IN  (*(volatile uint8_t*)0)
#define P2IE  (*(volatile uint8_t*)0)
#define P2IFG (*(volatile uint8_t*)0)
#define P2IES (*(volatile uint8_t*)0)
#define P5SEL (*(volatile uint8_t*)0)
#define P5DIR (*(volatile uint8_t*)0)
#define PJDIR (*(volatile uint8_t*)0)
#define PJOUT (*(volatile uint8_t*)0)

/* Bit definitions */
#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

/* LCD Control registers */
extern uint8_t emu_lcdbctl0;
extern uint8_t emu_lcdbctl1;
extern uint8_t emu_lcdbvctl;
extern uint16_t emu_lcdbpctl0;
extern uint16_t emu_lcdbpctl1;
extern uint8_t emu_lcdbmemctl;
extern uint8_t emu_lcdbiv;

#define LCDBCTL0   emu_lcdbctl0
#define LCDBCTL1   emu_lcdbctl1
#define LCDBVCTL   emu_lcdbvctl
#define LCDBPCTL0  emu_lcdbpctl0
#define LCDBPCTL1  emu_lcdbpctl1
#define LCDBMEMCTL emu_lcdbmemctl
#define LCDBIV     emu_lcdbiv
#define LCDBCPCTL  (*(volatile uint8_t*)0)
#define LCDNOCAPIFG 0x01

/* LCD Control bits */
#define LCDDIV0  0x0001
#define LCDDIV1  0x0002
#define LCDDIV2  0x0004
#define LCDDIV3  0x0008
#define LCDDIV4  0x0010
#define LCDPRE0  0x0020
#define LCD3MUX  0x0040
#define LCDSSEL  0x0080
#define LCDON    0x0001
#define LCDSON   0x0004
#define LCDCPEN  0x0001
#define LCD2B    0x0002
#define VLCD_2_60 0x00
#define VLCD_3_02 0x04
#define VLCD_3_44 0x08
#define LCDCLRM  0x0002
#define LCDDISP  0x0001

/* Watchdog */
#define WDTCTL (*(volatile uint16_t*)0)
#define WDTPW  0x5A00
#define WDTHOLD 0x0080
#define WDT_ADLY_250 0
#define SFRIE1 (*(volatile uint8_t*)0)
#define WDTIE  0x01
#define WDT_VECTOR 0

/* Other control registers */
#define UCSCTL7 (*(volatile uint16_t*)0)
#define REFCTL0 (*(volatile uint8_t*)0)
#define REFON   0x01
#define PMMCTL0 (*(volatile uint16_t*)0)
#define PMMPW   0xA500
#define PMMSWPOR 0x0004

/* RF registers (stubbed) */
#define RF1AIFERR (*(volatile uint8_t*)0)

/* ADC registers and bits */
#define ADC12CTL0 0x3FFF
#define ADC12ON   0x0010
#define ADC10CTL0_ 0x0740
#define ADC10CTL0 0
#define ADC10ON   0x0010
#define ADCISACTIVE 0

/* Status register bits */
#define LPM3_bits 0x00F0
#define GIE       0x0008
#define __bis_SR_register(x) do {} while(0)

/* Device ID */
#define DEVICEID (*(volatile uint16_t*)0x1A04)
#define DEVICEID6137 0x6137
#define DEVICEID6147 0x6147

/* LCD Memory - actual emulated buffers */
extern uint8_t emu_lcdm[13];
extern uint8_t emu_lcdbm[13];

#define LCDM1  emu_lcdm[0]
#define LCDBM1 emu_lcdbm[0]

/* Interrupt vectors (unused) */
#define PORT2_VECTOR 0

/* Update RTC from system time */
void emu_update_rtc(void);

/* Prevent firmware printf.h from overriding standard library */
#define printf_h_
#define printf_h_included_

/* Stub out MSP430-specific register macros */
#define sfrb(name, addr)
#define sfrw(name, addr)


#endif /* MSP430_STUBS_H */
