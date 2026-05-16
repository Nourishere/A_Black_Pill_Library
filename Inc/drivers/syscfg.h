/*
 * System Configuration (SYSCFG) controller header
 * Remap memory accessible in code area and manage external interrupts
 * Date: Sat May 16 05:38:49 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#ifndef SYSCFG_H
#define SYSCFG_H

#include <stdint.h>
#include <drivers/exti.h>

#define SYSCFG_BASE 0x40013800U

#define SYSCFG_MEMRMP	(*(volatile uint32_t *) (SYSCFG_BASE + 0x00))
#define SYSCFG_PMC	(*(volatile uint32_t *) (SYSCFG_BASE + 0x04))
#define SYSCFG_EXTICR1	(*(volatile uint32_t *) (SYSCFG_BASE + 0x08))
#define SYSCFG_EXTICR2	(*(volatile uint32_t *) (SYSCFG_BASE + 0x0C))
#define SYSCFG_EXTICR3	(*(volatile uint32_t *) (SYSCFG_BASE + 0x10))
#define SYSCFG_EXTICR4	(*(volatile uint32_t *) (SYSCFG_BASE + 0x14))
#define SYSCFG_CMPCR	(*(volatile uint32_t *) (SYSCFG_BASE + 0x20))

#define ADDR_SYSCFG_EXTICR1	((volatile uint32_t *) (SYSCFG_BASE + 0x08))
// Three memory types used by the device
typedef enum {
	main_flah, system_flash, sram
} memory_t;

/* Ports used in this device
 * NOTE: PF and PG are reserved on this device
 */
typedef enum {
	PA, PB, PC, PD, PE, PF, PG, PH
} port_t;

/*
 * TODO: come back to this once we reach the ADC module
 */
uint8_t SYSCFG_set_ADC_bit(void);

/*
 * Select a pin for an external interrupt to happen on
 *
 * port: A port_t type for the port number
 * pin: Pin number. Can't exceed 15 for this device.
 */
uint8_t SYSCFG_select_EXTI_pin(port_t port, uint8_t pin);

/*
 * Enable compensation cell control
 *
 * NOTE: You would need to call this function if you are running
 * 		the output buffer speed above 50 MHz with supply
 * 		voltage between 2.4 V and 3.6 V 
 */
static inline uint8_t SYSCFG_enable_compenstation(void);

/*
 * Disable compensation cell control
 */
static inline uint8_t SYSCFG_disable_compenstation(void);

#endif				//SYSCFG_H
