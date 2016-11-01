/*
 *  ETA/OS ATmega328 IO defs
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

#ifndef __IOM328_H_
#define __IOM328_H_

#define RAMEND 0x7FF
#define INTERNAL_RAMEND RAMEND
#define RAMSTART 0x100

/* interrupt defs */
#define CONFIG_ARCH_VECTORS 26

#define _VECTORS_SIZE (26*4)

#define EXT_IRQ0_NUM		    1
#define TIMER0_OVERFLOW_VECTOR_NUM 16
#define SPI_STC_VECTOR_NUM	   17
#define TWI_STC_VECTOR_NUM	   24
#define USART_RX_STC_NUM	   18
#define ADC_COMPLETED_NUM	   21
#define TIMER2_OVERFLOW_VECTOR_NUM  9

#define EXT_IRQ0_VECTOR irq_vector(1)
#define TIMER0_OVERFLOW_VECTOR irq_vector(16)
#define USART_RX_STC_VECTOR irq_vector(18)
#define SPI_STC_VECTOR irq_vector(17)
#define TWI_STC_VECTOR irq_vector(24)
#define ADC_COMPLETED_VECTOR irq_vector(21)
#define TIMER2_OVERFLOW_VECTOR irq_vector(9)

#define AVR_IRQ_FLAG 7
#define AVR_IRQ_BITS (1 << AVR_IRQ_FLAG)

#define AVR_INTERRUPT_FLAG AVR_IRQ_FLAG

/* stack defs */
#define AVR_STACK_LOW_ADDR 0x3D
#define AVR_STACK_HI_ADDR 0x3E
#define AVR_STATUS_ADDR 0x3F

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

#ifdef CONFIG_SIMUL_AVR

#define SIMO (*((volatile unsigned char*)0x20))
#define SIMI (*((volatile unsigned char*)0x21))
#define SIME (*((volatile unsigned char*)0x22))

#define STDOUT_PORT SIMO
#define STDIN_PORT SIMI
#endif /* CONFIG_SIMUL_AVR */

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

/* GPIO defs */
#define PINB MEM_IO8(0x23)
#define DDRB MEM_IO8(0x24)
#define PORTB MEM_IO8(0x25)

#define PINC MEM_IO8(0x26)
#define DDRC MEM_IO8(0x27)
#define PORTC MEM_IO8(0x28)

#define PIND MEM_IO8(0x29)
#define DDRD MEM_IO8(0x2A)
#define PORTD MEM_IO8(0x2B)

#define GPIO_PINS 24

/* USART defs */
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

/* SPI defs */
#define SS_GPIO		2
#define SCK_GPIO	5
#define MOSI_GPIO	3
#define MISO_GPIO	4

/* ADC */
#define ADC_PINS	8
#define ADMUX_MASK	0x7

#define ADMUX		IO_ADDR(0x7C)
#define MUX0		0
#define MUX1		1
#define MUX2		2
#define MUX3		3
#define ADLAR		5
#define REFS0		6
#define REFS1		7

#define ADCSRB		IO_ADDR(0x7B)
#define ADTS0		0
#define ADTS1		1
#define ADTS2		2
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

#endif /* __IOM328_H_ */

