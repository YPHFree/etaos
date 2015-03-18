/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/stdlib.h>
#include <etaos/string.h>
#include <etaos/mem.h>
#include <etaos/thread.h>
#include <etaos/evm.h>
#include <etaos/mem.h>
#include <etaos/mutex.h>
#include <etaos/time.h>
#include <etaos/tick.h>
#include <etaos/gpio.h>
#include <etaos/platform.h>
#include <etaos/ipm.h>

#include <etaos/sram/23k256.h>

static void *test_stack;
static uint8_t test_stack2[CONFIG_STACK_SIZE];

static struct ipm_queue ipm_q;
static uint8_t ee_value = 0x0;
static const char ee_output[] = "[%u][%s]: ee-byte read: %u\n";

static const char sram_test[] = "sram test";
static const char ee_test[]   = "ee test";

#define EE_BYTE_ADDR 0x20
#define EE_STRING_ADDR 0x40

#define SRAM_BYTE_ADDR 0x100
#define SRAM_STRING_ADDR 0x200

extern int ee_stress_read_byte(uint8_t addr, uint8_t *rb);
extern int ee_stress_write_byte(uint8_t addr, uint8_t byte);
extern int ee_stress_write(uint8_t addr, const void *buff, size_t len);
extern int ee_stress_read(uint8_t addr, void *buff, size_t len);

extern int sram_stress_read_byte(uint16_t addr, uint8_t *store);
extern int sram_stress_write_byte(uint16_t addr, uint8_t byte);
extern int sram_stress_read(uint16_t addr, void *buff, size_t len);
extern int sram_stress_write(uint16_t addr, const void *buff, size_t len);

static char *current_thread_name(void)
{
	return current_thread()->name;
}

THREAD(test_th_handle2, arg)
{
	unsigned char sram_readback;
	unsigned long rand;
	float sram_entry = 3.1415F;

	sram_stress_write_byte(SRAM_BYTE_ADDR, 0x78);
	sram_stress_write(SRAM_STRING_ADDR, &sram_entry, sizeof(sram_entry));

	while(true) {
		sram_stress_read_byte(SRAM_BYTE_ADDR, &sram_readback);
		rand = random_m(100);
		printf("[2][%s]: SRAM: %u :: RAND: %u\n",
				current_thread_name(), sram_readback, rand);
		sleep(1000);
	}
}

THREAD(test_th_handle, arg)
{
	int fd;
	struct ipm msg;
	uint8_t readback;
	float sram_data;
	char ee_string[sizeof(ee_test)];

	thread_create("test-2", &test_th_handle2, NULL, CONFIG_STACK_SIZE,
			test_stack2, 150);

	while(true) {
		ee_stress_read_byte(EE_BYTE_ADDR, &readback);
		printf(ee_output, 1, current_thread_name(), readback);

		ipm_get_msg(&ipm_q, &msg);
		ipm_reset_queue(&ipm_q);

		fd = open("atm-usart", _FDEV_SETUP_RW);
		write(fd, msg.data, msg.len);
		close(fd);

		sram_stress_read(SRAM_STRING_ADDR, &sram_data,
				sizeof(sram_data));
		ee_stress_read(EE_STRING_ADDR, ee_string, sizeof(ee_string));
		printf("[1][%s]: SRAM::EEPROM %f::%s\n", current_thread_name(),
				sram_data, ee_string);

		sleep(500);
	}
}

int main(void)
{
	const char * ip_msg = "IPM message\n";
	bool value = true;
	uint8_t readback = 0;

	ipm_queue_init(&ipm_q, 2);
	ee_stress_write_byte(EE_BYTE_ADDR, 0xAC);
	ee_stress_write(EE_STRING_ADDR, ee_test, strlen(ee_test)+1);

	test_stack = kzalloc(CONFIG_STACK_SIZE);
	thread_create("test-1", &test_th_handle, NULL,
			CONFIG_STACK_SIZE, test_stack, 150);

	pgpio_pin_request(13);
	pgpio_direction_output(13, false);
	pgpio_pin_release(13);

	while(true) {
		ipm_post_msg(&ipm_q, ip_msg, strlen(ip_msg));

		pgpio_pin_request(13);
		pgpio_write_pin(13, value);
		pgpio_pin_release(13);
		value = !value;

		ee_stress_read_byte(EE_BYTE_ADDR, &readback);
		ee_value += 1;
		ee_stress_write_byte(EE_BYTE_ADDR, ee_value);
		printf("[%u][%s]:   ee-byte read: %u\n", 0,
				current_thread_name(), readback);
		printf("[0][%s]:   Memory available: %u\n",
				current_thread_name(),
				mm_heap_available());

		sleep(500);
	}
	return 0;
}
