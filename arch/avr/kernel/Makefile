obj-n += dummy.o
obj-$(CONFIG_EXT_MEM) += mem.o
obj-$(CONFIG_ATMEGA_ANALOG:m=y) += adc.o
obj-$(CONFIG_I2C_ATMEGA:m=y) += i2c.o
obj-$(CONFIG_ATMEGA_SPI:m=y) += spi.o
obj-$(CONFIG_ATMEGA_USART:m=y) += usart.o
obj-$(CONFIG_SIMUL_AVR) += simulavr.o
obj-$(CONFIG_IRQ_SUPPORT) += irq.o extirq.o
obj-$(CONFIG_TIMER) += timer.o
obj-$(CONFIG_ARCH_PWM) += pwm.o
obj-$(CONFIG_SCHED) += sched.o context.o
obj-$(CONFIG_ARCH_POWER_SAVE) += power.o
obj-$(CONFIG_HRTIMER) += hrtimer.o
obj-$(CONFIG_PYTHON) += python/
obj-$(CONFIG_ATMEGA_EE:m=y) += eeprom.o
obj-$(CONFIG_WATCHDOG) += watchdog.o
obj-y += cpu.o
extra-y += start.o devinit.o init.o
