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
#include <etaos/23k256.h>

static void *test_stack;
static uint8_t test_stack2[CONFIG_STACK_SIZE];

static struct ipm_queue ipm_q;
static uint8_t ee_value = 0x0;
static const char *ee_output = "[%u][%s]: %u\n";

#define EE_ADDR 0x20

extern int ee_stress_read_byte(uint8_t addr, uint8_t *rb);
extern int ee_stress_write_byte(uint8_t addr, uint8_t byte);

extern int sram_stress_read_byte(uint16_t addr, uint8_t *store);
extern int sram_stress_write_byte(uint16_t addr, uint8_t byte);

THREAD(test_th_handle2, arg)
{
	unsigned char sram_readback;
	unsigned long rand;

	sram_23k256_init();
	sram_stress_write_byte(0x100, 0x78);
	
	while(true) {
		sram_stress_read_byte(0x100, &sram_readback);
		rand = random_m(100);
		printf("[2][tst2]: SRAM: %u :: RAND: %u\n", sram_readback, rand);
		sleep(1000);
	}
}

THREAD(test_th_handle, arg)
{
	int fd;
	struct ipm msg;
	uint8_t readback;

	thread_create("tst2", &test_th_handle2, NULL, CONFIG_STACK_SIZE, 
			test_stack2, 150);

	while(true) {
		ee_stress_read_byte(EE_ADDR, &readback);
		printf(ee_output, 1, "tst1", readback);

		ipm_get_msg(&ipm_q, &msg);
		ipm_reset_queue(&ipm_q);

		fd = open("atm-usart", _FDEV_SETUP_RW);
		write(fd, msg.data, msg.len);
		close(fd);
	}
}

int main(void)
{
	const char * ip_msg = "IPM message\n";
	bool value = true;
	uint8_t readback = 0;

	ipm_queue_init(&ipm_q, 2);
	ee_stress_write_byte(EE_ADDR, 0xAC);

	test_stack = kzalloc(CONFIG_STACK_SIZE);
	thread_create("tst", &test_th_handle, NULL,
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

		ee_stress_read_byte(EE_ADDR, &readback);
		ee_value += 1;
		ee_stress_write_byte(EE_ADDR, ee_value);
		printf(ee_output, 0, "main", readback);
		printf("[0][main]: Memory available: %u\n", 
				mm_heap_available());

		sleep(500);
	}
	return 0;
}

