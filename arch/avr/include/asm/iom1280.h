/*
 *  ETA/OS ATmega1280 IO defs
 *  Copyright (C) 2014, 2015   Michel Megens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __IO1280_H__
#define __IO1280_H__

#define RAMEND INTERNAL_RAMEND + CONFIG_EXT_MEM
#define INTERNAL_RAMEND 0x21FF
#define RAMSTART 0x200
#define EXTERNAL_RAMSTART 0x2200

#if defined(CONFIG_ATMEGA2560)
#define AVR_22BIT_PC 1
#endif

/* interrupt defs */
#define CONFIG_ARCH_VECTORS 57

#define _VECTORS_SIZE (57*4)

#define EXT_IRQ0_VECTOR_NUM			 1
#define EXT_IRQ1_VECTOR_NUM			 2
#define EXT_IRQ2_VECTOR_NUM			 3
#define EXT_IRQ3_VECTOR_NUM			 4
#define EXT_IRQ4_VECTOR_NUM			 5
#define EXT_IRQ5_VECTOR_NUM			 6
#define EXT_IRQ6_VECTOR_NUM			 7
#define EXT_IRQ7_VECTOR_NUM			 8
#define TIMER0_OVERFLOW_VECTOR_NUM	23
#define SPI_STC_VECTOR_NUM		24
#define TWI_STC_VECTOR_NUM		39
#define USART_RX_STC_NUM		25
#define ADC_COMPLETED_NUM		29
#define TIMER2_OVERFLOW_VECTOR_NUM	15

#define TIMER1_CAPT_VECTOR_NUM 16
#define TIMER1_OCA_VECTOR_NUM 17
#define TIMER1_OCB_VECTOR_NUM 18
#define TIMER1_OCC_VECTOR_NUM 19
#define TIMER3_OCA_VECTOR_NUM 32
#define TIMER3_OCB_VECTOR_NUM 33
#define TIMER3_OCC_VECTOR_NUM 34
#define TIMER4_OCA_VECTOR_NUM 42
#define TIMER4_OCB_VECTOR_NUM 43
#define TIMER4_OCC_VECTOR_NUM 44
#define TIMER5_OCA_VECTOR_NUM 47
#define TIMER5_OCB_VECTOR_NUM 48
#define TIMER5_OCC_VECTOR_NUM 49

#define USART1_RX_COMPLETE_VECTOR_NUM 36
#define USART1_UDRE_VECTOR_NUM 37

#define EXT_IRQ0_VECTOR irq_vector(1)
#define EXT_IRQ1_VECTOR irq_vector(2)
#define EXT_IRQ2_VECTOR irq_vector(3)
#define EXT_IRQ3_VECTOR irq_vector(4)
#define EXT_IRQ4_VECTOR irq_vector(5)
#define EXT_IRQ5_VECTOR irq_vector(6)
#define EXT_IRQ6_VECTOR irq_vector(7)
#define EXT_IRQ7_VECTOR irq_vector(8)
#define TIMER0_OVERFLOW_VECTOR irq_vector(23)
#define SPI_STC_VECTOR irq_vector(24)
#define TWI_STC_VECTOR irq_vector(39)
#define USART_RX_STC_VECTOR irq_vector(25)
#define ADC_COMPLETED_VECTOR irq_vector(29)
#define TIMER2_OVERFLOW_VECTOR irq_vector(15)
#define USART1_RX_COMPLETE_VECTOR irq_vector(36)
#define USART1_UDRE_VECTOR irq_vector(37)

#define TIMER1_CAPT_VECTOR irq_vector(16)
#define TIMER1_OCA_VECTOR irq_vector(17)
#define TIMER1_OCB_VECTOR irq_vector(18)
#define TIMER1_OCC_VECTOR irq_vector(19)
#define TIMER3_OCA_VECTOR irq_vector(32)
#define TIMER3_OCB_VECTOR irq_vector(33)
#define TIMER3_OCC_VECTOR irq_vector(34)
#define TIMER4_OCA_VECTOR irq_vector(42)
#define TIMER4_OCB_VECTOR irq_vector(43)
#define TIMER4_OCC_VECTOR irq_vector(44)
#define TIMER5_OCA_VECTOR irq_vector(47)
#define TIMER5_OCB_VECTOR irq_vector(48)
#define TIMER5_OCC_VECTOR irq_vector(49)

#define COMC 3
#define COMB 5
#define COMA 7

#define AVR_IRQ_FLAG 7
#define AVR_IRQ_BITS (1 << AVR_IRQ_FLAG)

/* stack defs */
#define AVR_STACK_LOW_ADDR 0x3D
#define AVR_STACK_HI_ADDR 0x3E
#define AVR_STATUS_ADDR 0x3F

#define SPL MEM_IO8(0x3D)
#define SPH MEM_IO8(0x3E)

#define AVR_INTERRUPT_FLAG 7

#define AVR_HAVE_RAMPZ 1
#define AVR_RAMPZ_ADDR 0x3B

#define AVR_HAVE_EIND  1
#define AVR_EIND_ADDR 0x3C

#define XJMP jmp
#define XCALL call

#if !defined(__zero_reg__)
    #if defined(CONFIG_AVRTINY)
        #define __zero_reg__ r17
    #else
        #define __zero_reg__ r1
    #endif
#endif

#define cli() __asm__ __volatile__("cli")
#define sei() __asm__ __volatile__("sei")

#define SREG (*((volatile unsigned char*)0x5F))

/* USART1 */
#define UCSR1A  MEM_IO8(0xC8)
#define RXC1    7
#define TXC1    6
#define UDRE1   5
#define FE1     4
#define DOR1    3
#define UPE1    2
#define U2X1    1
#define MPCM1   0

#define UCSR1B  MEM_IO8(0XC9)
#define RXCIE1  7
#define TXCIE1  6
#define UDRIE1  5
#define RXEN1   4
#define TXEN1   3
#define UCSZ12  2
#define RXB81   1
#define TXB81   0

#define UCSR1C  MEM_IO8(0xCA)
#define UMSEL11 7
#define UMSEL10 6
#define UPM11   5
#define UPM10   4
#define USBS1   3
#define UCSZ11  2
#define UCSZ10  1
#define UCPOL1  0

#define UBRR1   MEM_IO16(0xCC)

#define UBRR1L  MEM_IO8(0xCC)
#define UBRR1H  MEM_IO8(0xCD)
#define UDR1    MEM_IO8(0XCE)

#ifdef CONFIG_SIMUL_AVR

#define SIMO (*((volatile unsigned char*)0x20))
#define SIMI (*((volatile unsigned char*)0x21))
#define SIME (*((volatile unsigned char*)0x22))

#define STDOUT_PORT SIMO
#define STDIN_PORT SIMI
#endif /* CONFIG_SIMUL_AVR */

#define EXT_IRQ0_PIN 24
#define EXT_IRQ1_PIN 25
#define EXT_IRQ2_PIN 26
#define EXT_IRQ3_PIN 27
#define EXT_IRQ4_PIN 36
#define EXT_IRQ5_PIN 37
#define EXT_IRQ6_PIN 38
#define EXT_IRQ7_PIN 39

/* External interrupts */
#define EICRA   MEM_IO8(0x69)
#define ISC31   7
#define ISC30   6
#define ISC21   5
#define ISC20   4
#define ISC11   3
#define ISC10   2
#define ISC01   1
#define ISC00   0

#define EICRB   MEM_IO8(0x6A)
#define ISC71   7
#define ISC70   6
#define ISC61   5
#define ISC60   4
#define ISC51   3
#define ISC50   2
#define ISC41   1
#define ISC40   0

#define EIMSK   MEM_IO8(0x3D)
#define INT7    7
#define INT6    6
#define INT5    5
#define INT4    4
#define INT3    3
#define INT2    2
#define INT1    1
#define INT0    0

/* TIMER 0 */
#define TCCR0A MEM_IO8(0x44)
#define WGM00  0x1
#define WGM01  0x2
#define COM0B0 0x10
#define COM0B1 0x20
#define COM0A0 0x40
#define COM0A1 0x80

#define TCCR0B MEM_IO8(0x45)
#define CS00  0x1
#define CS01  0x2
#define CS02  0x4
#define WGM02 0x8
#define FOC0B 0x40
#define FOC0A 0x80

#define TCNT0  MEM_IO8(0x46)
#define OCR0A  MEM_IO8(0x47)
#define OCR0B  MEM_IO8(0x48)

#define TIMSK0 MEM_IO8(0x6E)
#define TOIE0  0x1
#define OCIE0A 0x2
#define OCIE0B 0x4

#define TIFR0  MEM_IO8(0x35)
#define TOV   0x1
#define OCF0A 0x2
#define OCF0B 0x4

/* TIMER2 */
#define TCCR2A		IO_ADDR(0xB0)
#define WGM20		0
#define WGM21		1
#define COM2B0		4
#define COM2B1		5
#define COM2A0		6
#define COM2A1		7

#define TCCR2B		IO_ADDR(0xB1)
#define CS20		0
#define CS21		1
#define CS22		2
#define WGM22		3
#define FOC2B		6
#define FOC2A		7

#define TCNT2		IO_ADDR(0xB2)
#define OCR2A		IO_ADDR(0xB3)
#define OCR2B		IO_ADDR(0xB4)
#define ASSR		IO_ADDR(0xB6)
#define TIMSK2		IO_ADDR(0x70)
#define TOIE2		0
#define OCIE2A		1
#define OCIE2B		2

#define TIFR2		IO_ADDR(0x37)
#define TOV2		0
#define OCF2A		1
#define OCF2B		2

#define GTCCR		IO_ADDR(0x43)

/* TIMER 1, 3, 4, 5 (PWM) */

/* OCR registers */
#define OCR1A   MEM_IO16(0x88)
#define OCR1B   MEM_IO16(0x8A)
#define OCR1C   MEM_IO16(0x8C)

#define OCR3A   MEM_IO16(0x98)
#define OCR3B   MEM_IO16(0x9A)
#define OCR3C   MEM_IO16(0x9C)

#define OCR4A   MEM_IO16(0xA8)
#define OCR4B   MEM_IO16(0xAA)
#define OCR4C   MEM_IO16(0xAC)

#define OCR5A   MEM_IO16(0x128)
#define OCR5B   MEM_IO16(0x12A)
#define OCR5C   MEM_IO16(0x12C)

#define TCCR1A  MEM_IO8(0x80)
#define COM1A1  7
#define COM1A0  6
#define COM1B1  5
#define COM1B0  4
#define COM1C1  3
#define COM1C0  2
#define WGM11   1
#define WGM10   0

#define TCCR1B  MEM_IO8(0x81)
#define ICNC1   7
#define ICES1   6
#define WGM13   4
#define WGM12   3
#define CS12    2
#define CS11    1
#define CS10    0

#define TCCR1C  MEM_IO8(0x82)
#define FOC1A   7
#define FOC1B   6
#define FOC1C   5

#define TCCR3A  MEM_IO8(0x90)
#define COM3A1  7
#define COM3A0  6
#define COM3B1  5
#define COM3B0  4
#define COM3C1  3
#define COM3C0  2
#define WGM31   1
#define WGM30   0

#define TCCR3B  MEM_IO8(0x91)
#define ICNC3   7
#define ICES3   6
#define WGM33   4
#define WGM32   3
#define CS32    2
#define CS31    1
#define CS30    0

#define TCCR3C  MEM_IO8(0x92)
#define FOC3A   7
#define FOC3B   6
#define FOC3C   5

#define TCCR4A  MEM_IO8(0xA0)
#define COM4A1  7
#define COM4A0  6
#define COM4B1  5
#define COM4B0  4
#define COM4C1  3
#define COM4C0  2
#define WGM41   1
#define WGM40   0

#define TCCR4B  MEM_IO8(0xA1)
#define ICNC4   7
#define ICES4   6
#define WGM43   4
#define WGM42   3
#define CS42    2
#define CS41    1
#define CS40    0

#define TCCR4C  MEM_IO8(0xA2)
#define FOC4A   7
#define FOC4B   6
#define FOC4C   5

#define TCCR5A  MEM_IO8(0x120)
#define COM5A1  7
#define COM5A0  6
#define COM5B1  5
#define COM5B0  4
#define COM5C1  3
#define COM5C0  2
#define WGM51   1
#define WGM50   0

#define TCCR5B  MEM_IO8(0x121)
#define ICNC5   7
#define ICES5   6
#define WGM53   4
#define WGM52   3
#define CS52    2
#define CS51    1
#define CS50    0

#define TCCR5C  MEM_IO8(0x122)
#define FOC5A   7
#define FOC5B   6
#define FOC5C   5

/* Interrupt flags for timer 1, 3, 4, 5 */
#define TIFR1   MEM_IO8(0x16)
#define ICF1    5
#define OCF1C   3
#define OCF1B   2
#define OCF1A   1
#define TOV1    0

#define TIFR3   MEM_IO8(0x18)
#define ICF3    5
#define OCF3C   3
#define OCF3B   2
#define OCF3A   1
#define TOV3    0

#define TIFR4   MEM_IO8(0x19)
#define ICF4    5
#define OCF4C   3
#define OCF4B   2
#define OCF4A   1
#define TOV4    0

#define TIFR5   MEM_IO8(0x1A)
#define ICF5    5
#define OCF5C   3
#define OCF5B   2
#define OCF5A   1
#define TOV5    0

#define TIMSK1  MEM_IO8(0x6F)
#define ICIE1   5
#define OCIE1C  3
#define OCIE1B  2
#define OCIE1A  1
#define TOIE1   0

#define TIMSK3  MEM_IO8(0x71)
#define ICIE3   5
#define OCIE3C  3
#define OCIE3B  2
#define OCIE3A  1
#define TOIE3   0

#define TIMSK4  MEM_IO8(0x72)
#define ICIE4   5
#define OCIE4C  3
#define OCIE4B  2
#define OCIE4A  1
#define TOIE4   0

#define TIMSK5  MEM_IO8(0x73)
#define ICIE5   5
#define OCIE5C  3
#define OCIE5B  2
#define OCIE5A  1
#define TOIE5   0

#define TCNT1   MEM_IO16(0x84)
#define ICR1    MEM_IO16(0x86)

#define ICR3    MEM_IO16(0x96)
#define TCNT3   MEM_IO16(0x94)

#define TCNT4   MEM_IO16(0xA4)
#define ICR4    MEM_IO16(0xA6)

#define TCNT5   MEM_IO16(0x124)
#define ICR5    MEM_IO16(0x126)

/* External memory interface */
#define XMCRA MEM_IO8(0x74)
#define SRE		7
#define XMCRB MEM_IO8(0x75)

/* GPIO defs */
#define P0		0
#define P1		1
#define P2		2
#define P3		3
#define P4		4
#define P5		5
#define P6		6
#define P7		7

#define PINA MEM_IO8(0x20)
#define DDRA MEM_IO8(0x21)
#define PORTA MEM_IO8(0x22)

#define PINB MEM_IO8(0x23)
#define DDRB MEM_IO8(0x24)
#define PORTB MEM_IO8(0x25)

#define PINC MEM_IO8(0x26)
#define DDRC MEM_IO8(0x27)
#define PORTC MEM_IO8(0x28)

#define PIND MEM_IO8(0x29)
#define DDRD MEM_IO8(0x2A)
#define PORTD MEM_IO8(0x2B)

#define PINE MEM_IO8(0x2C)
#define DDRE MEM_IO8(0x2D)
#define PORTE MEM_IO8(0x2E)

#define PINF MEM_IO8(0x2F)
#define DDRF MEM_IO8(0x30)
#define PORTF MEM_IO8(0x31)

#define PING MEM_IO8(0x32)
#define DDRG MEM_IO8(0x33)
#define PORTG MEM_IO8(0x34)

#define PINH MEM_IO8(0x100)
#define DDRH MEM_IO8(0x101)
#define PORTH MEM_IO8(0x102)

#define PINJ MEM_IO8(0x103)
#define DDRJ MEM_IO8(0x104)
#define PORTJ MEM_IO8(0x105)

#define PINK MEM_IO8(0x106)
#define DDRK MEM_IO8(0x107)
#define PORTK MEM_IO8(0x108)

#define PINL MEM_IO8(0x109)
#define DDRL MEM_IO8(0x10A)
#define PORTL MEM_IO8(0x10B)

#define GPIO_PINS 87

/* USART defs */
#define UBRR0 MEM_IO16(0xC4)
#define UBRR0L MEM_IO8(0xC4)
#define UBRR0H MEM_IO8(0xC5)
#define UDR0 MEM_IO8(0xC6)

#define UCSR0A MEM_IO8(0xC0)
#define UCSR0B MEM_IO8(0xC1)
#define UCSR0C MEM_IO8(0xC2)

/* TWI defs */
#define TWBR  MEM_IO8(0xB8)
#define TWCR  MEM_IO8(0xBC)
#define TWINT	7
#define TWEA	6
#define TWSTA	5
#define TWSTO	4
#define TWWC	3
#define TWEN	2
#define TWIE	0

#define TWSR  MEM_IO8(0xB9)
#define TWPS1	1
#define TWPS0	0
#define I2C_NOINFO 0xF8

#define TWDR  MEM_IO8(0xBB)
#define TWAR  MEM_IO8(0xBA)
#define TWGCE 	0

#define TWAMR MEM_IO8(0xBD)

#if F_CPU == 8000000
#define TWBR_VAL 32
#define TWSR_VAL (TWSR & 0xF8)
#elif F_CPU == 16000000
#define TWBR_VAL 72
#define TWSR_VAL (TWSR & 0xF8)
#endif

/* SPI pins */
#define SS_GPIO		 8
#define SCK_GPIO	 9
#define MOSI_GPIO	10
#define MISO_GPIO	11

/* ADC */
#define ADC_PINS	16
#define ADMUX_MASK	0xF

#define ADMUX		IO_ADDR(0x7C)
#define MUX0		0
#define MUX1		1
#define MUX2		2
#define MUX3		3
#define MUX4		4
#define ADLAR		5
#define REFS0		6
#define REFS1		7

#define ADCSRB		IO_ADDR(0x7B)
#define ADTS0		0
#define ADTS1		1
#define ADTS2		2
#define MUX5		3
#define ACME		6

#define ADCSRA		IO_ADDR(0x7A)
#define ADPS0		0
#define ADPS1		1
#define ADPS2		2
#define ADIE		3
#define ADIF		4
#define ADATE		5
#define ADSC		6
#define ADEN		7

#define ADCL		IO_ADDR(0x78)
#define ADCH		IO_ADDR(0x79)

/* POWER MANAGEMENT */
#define SMCR		MEM_IO8(0x33)

#define SE		0
#define SM0		1
#define SM1		2
#define SM2		3

#ifndef __ASSEMBLER__
CDECL
extern void avr_sre(void);
CDECL_END
#endif

#endif /* __IO1280_H__ */
