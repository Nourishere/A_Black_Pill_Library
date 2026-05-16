/*
 * System Configuration (SYSCFG) controller source 
 * Remap memory accessible in code area and manage external interrupts
 * Date: Sat May 16 05:38:49 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#include <drivers/syscfg.h>

/*
 * TODO: come back to this once we reach the ADC module
 */
uint8_t SYSCFG_set_ADC_bit(void)
{
	return 0;
}

/*
 * Select a pin for an EXTIx to happen on
 *
 * port: A port_t type for the port number
 * pin: Pin number. Can't exceed 15 for this device.
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t SYSCFG_select_EXTI_pin(port_t port, uint8_t pin)
{
	if (port > PH || port == PF || port == PG || pin > 15)
		return 1;

	uint32_t reg_idx = pin >> 2;
	uint8_t reg_offset = pin % 4;

	*(ADDR_SYSCFG_EXTICR1 + reg_idx) |= (port << reg_offset);

	return 0;
}

/*
 * Enable compensation cell control
 *
 * NOTE: You would need to call this function if you are running
 * 		the output buffer speed above 50 MHz with supply
 * 		voltage between 2.4 V and 3.6 V 
 *
 * Return 0 upon success and 1 otherwise 
 *
 */
static inline uint8_t SYSCFG_enable_compenstation(void)
{
	uint32_t timer = 10000;
	SYSCFG_CMPCR |= 0x1;

	while (!((SYSCFG_CMPCR >> 8) & 0x01)) {
		if (--timer == 0)
			return 1;
	}
	return 0;
}

/*
 * Disable compensation cell control
 *
 * Return 0 upon success and 1 otherwise 
 *
 */
static inline uint8_t SYSCFG_disable_compenstation(void)
{
	uint32_t timer = 10000;
	SYSCFG_CMPCR &= ~0x1;

	while ((SYSCFG_CMPCR >> 8) & 0x01) {
		if (--timer == 0)
			return 1;
	}
	return 0;
}
