/*
 *  ETA/OS - ATmega GPIO driver
 *  Copyright (C) 2014   Michel Megens <dev@michelmegens.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/gpio.h>

#include <asm/io.h>

#define PINS_PER_PORT 8

static volatile void *atmega_gpio_ports[] = {
#ifdef PORTA
	&PORTA,
#endif
#ifdef PORTB
	&PORTB,
#endif
#ifdef PORTC
	&PORTC,
#endif
#ifdef PORTD
	&PORTD,
#endif
#ifdef PORTE
	&PORTE,
#endif
};

static volatile void *atmega_gpio_pins[] = {
#ifdef PORTA
	&PINA,
#endif
#ifdef PORTB
	&PINB,
#endif
#ifdef PORTC
	&PINC,
#endif
#ifdef PORTD
	&PIND,
#endif
#ifdef PORTE
	&PINE,
#endif
};

static volatile void *atmega_gpio_ddrs[] = {
#ifdef PORTA
	&DDRA,
#endif
#ifdef PORTB
	&DDRB,
#endif
#ifdef PORTC
	&DDRC,
#endif
#ifdef PORTD
	&DDRD,
#endif
#ifdef PORTE
	&DDRE,
#endif
};

static inline volatile uint8_t *atmega_pin_to_ddr(struct gpio_pin *pin)
{
	uint8_t idx;

	idx = pin->nr / PINS_PER_PORT;
	return (volatile uint8_t*)atmega_gpio_ddrs[idx];
}

static inline volatile uint8_t *atmega_pin_to_pin_addr(struct gpio_pin *pin)
{
	uint8_t idx;

	idx = pin->nr / PINS_PER_PORT;
	return (volatile uint8_t*)atmega_gpio_pins[idx];
}

static inline volatile uint8_t *atmega_pin_to_port(struct gpio_pin *pin)
{
	uint8_t idx;

	idx = pin->nr / PINS_PER_PORT;
	return (volatile uint8_t*)atmega_gpio_ports[idx];
}

static inline uint8_t atmega_pin_index(struct gpio_pin *pin)
{
	return (uint8_t)(pin->nr % PINS_PER_PORT);
}

static int atmega_set_pin(struct gpio_chip *chip, int val, uint16_t nr)
{
	uint8_t bit;
	volatile uint8_t *idx;
	struct gpio_pin *pin;

	pin = &chip->pins[nr];
	idx = atmega_pin_to_port(pin);
	bit = atmega_pin_index(pin);

	if(val)
		*idx |= 1 << bit;
	else
		*idx &= ~(1 << bit);

	return 0;
}

static int atmega_get_pin(struct gpio_chip *chip, uint16_t nr)
{
	uint8_t bit;
	volatile uint8_t *idx;
	struct gpio_pin *pin;
	int retval;

	pin = &chip->pins[nr];
	idx = atmega_pin_to_pin_addr(pin);
	bit = atmega_pin_index(pin);

	retval = *idx;
	retval &= (1<<bit);

	return !!retval;
}

static int atmega_dir_out(struct gpio_chip *chip, int val, uint16_t nr)
{
	uint8_t bit;
	volatile uint8_t *idx;
	struct gpio_pin *pin;

	pin = &chip->pins[nr];
	idx = atmega_pin_to_ddr(pin);
	bit = atmega_pin_index(pin);

	*idx |= 1 << bit;
	return atmega_set_pin(chip, val, nr);
}

static int atmega_dir_in(struct gpio_chip *chip, uint16_t nr)
{
	uint8_t bit;
	volatile uint8_t *idx;
	struct gpio_pin *pin;

	pin = &chip->pins[nr];
	idx = atmega_pin_to_ddr(pin);
	bit = atmega_pin_index(pin);

	*idx &= ~(1 << bit);

	return 0;
}

static int atmega_get_dir(struct gpio_chip *chip, uint16_t nr)
{
	uint8_t bit;
	volatile uint8_t *idx;
	struct gpio_pin *pin;
	int dir;

	pin = &chip->pins[nr];
	idx = atmega_pin_to_ddr(pin);
	bit = atmega_pin_index(pin);

	dir = *idx;
	dir &= 1 << bit;
	return !dir;
}

static struct gpio_chip atmega_gpio_chip = {
	.get = &atmega_get_pin,
	.set = &atmega_set_pin,
	.direction_output = &atmega_dir_out,
	.direction_input = &atmega_dir_in,
	.get_direction = &atmega_get_dir,
};

void atmega_init_gpio()
{
	int err;
	struct gpio_pin *pin;

	err = gpio_chip_init(&atmega_gpio_chip, GPIO_PINS);
	if(!err) {
		for(err = 0; err < GPIO_PINS; err++) {
			pin = gpio_chip_to_pin(&atmega_gpio_chip, err);
			gpio_pin_init(&atmega_gpio_chip, pin, err, 0);
		}
	} else {
		return;
	}

	gpio_set_sys_chip(&atmega_gpio_chip);
	return;
}
