/*
 * Power Controller header
 * The power controller configures the electrical characterstics of the board
 * It also configures the power mode of operations (sleep, standby, etc)
 *
 * Date: Tue Sep 22 12:51:14 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#ifndef PWR_H
#define PWR_H

#include <stdint.h>
#include <drivers/rcc.h>
#include <drivers/flash_ob.h>

#define PWR_BASE 0x40007000

#define PWR_CR		(*(volatile uint32_t *) (PWR_BASE + 0x00))
#define PWR_CSR		(*(volatile uint32_t *) (PWR_BASE + 0x04))

/*
 * The device's voltage regulator has four operating modes:
 * ON:
 * 	Main regulator mode (MR)
 * 	Low power regulartor (LPR)
 * 	Power-down
 * OFF
 */

/*
 * Set up the regulator's scale
 *
 * CAUTION: This function turns off the PLL as part of configuration
 * NOTE: For this function to take effect, either the HSI or the HSE
 * clock must be selected as clock source.
 *
 * scale: Either scale mode 2 or scale mode 3 should be used
 * 	  According to the DS, the typical voltage values for
 * 	  the scales are the following:
 *
 * 	  2 - 1.26 V
 * 	  3 - 1.32 V
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t PWR_set_regulator_scale(uint8_t scale);

/*
 * Set the threshold level for the Brown-out-reset (BOR) module
 *
 * level: Can be 1, 2, 3 or 0 for off
 * 	  According to the DS, the typical voltage values for
 * 	  the levels are the following (on the rising edge):
 *
 * 	  1 - 2.29 V
 * 	  2 - 2.59 V
 * 	  3 - 2.92 V
 *
 * Return 0 upn success and 1 otherwise
 *
 */
static inline uint8_t PWR_set_BOR_reset_level(uint8_t level)
{
	return OB_set_BOR_level(level);
}

/*
 * Enable the PVD (programmable voltage detector)
 */
static inline void PWR_enable_PVD(void)
{
	PWR_CR |= 1 << 0x4;
}

/*
 * Disable the PVD module
 */
static inline void PWR_disable_PVD(void)
{
	PWR_CR &= ~(1 << 0x4);
}

/*
 * Return whether VDD is higher than PVD thershold (1)
 * or is not (0)
 */
static inline uint8_t PWR_read_PVD_state(void)
{
	return (PWR_CR >> 2) & 0x1;
}

/*
 * Set the PVD threshold value
 *
 * threshold: The voltage threshold detected by the PVD
 * 	      Values are from 2.2 - 2.9 (0.1 V increment)
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t PWR_set_PVD(float threshold);

#endif				// PWR_H
