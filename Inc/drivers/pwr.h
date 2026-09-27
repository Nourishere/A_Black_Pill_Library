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
#include <drivers/exti.h>
#include <core/nvic.h>
#include <core/scb.h>

#define PWR_BASE 0x40007000

#define PWR_CR		(*(volatile uint32_t *) (PWR_BASE + 0x00))
#define PWR_CSR		(*(volatile uint32_t *) (PWR_BASE + 0x04))

/*
 * The device's has two voltage regulators
 * 	Main regulator mode (MR)
 * 	Low power regulator (LPR)
 * Each regulator has two modes of opeartion:
 * 	Normal
 * 	Low Power
 */
typedef enum {
	main, main_lp, low_power, low_power_lp
} regulator_mode_t;

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
	return (PWR_CSR >> 2) & 0x1;
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

/*
 * The STM32f4 has three low-power modes:
 * 	Sleep (sleep now or sleep-on exit)
 * 	Stop
 * 	Standby.
 * The Sleep mode corresponds to the DEEPSLEEP bit set in the SCR
 * register.
 * The Stop mode corresponds to the DEEPSLEEP bit set in the SCR
 * register. Peripheral clocks are gated. The voltage regulator can be
 * configured either in normal or low-power mode.
 * The Standby mode corresponds to the DEEPSLEEP bit set in the SCR
 * retister. The 1.2 V domain is powered off. All the oscillators are
 * turned off.
 */

/*
 * Enter Sleep mode
 *
 * entry: 0 for event entry (issue a WFE instruction)
 * 	  1 for interrupt entry (issue a WFI instruction)
 *
 * NOTE: Waiting on an event also waits on interrupts
 */
uint8_t PWR_enter_sleep_mode(uint8_t entry);

/*
 * Enter Sleep-on-exit mode
 *
 * NOTE: See `PWR_enter_sleep_mode` for more information
 */
uint8_t PWR_enter_sleep_on_exit_mode(uint8_t entry);

/*
 * Enter Stop mode
 *
 * NOTE: Stop mode is M4 deepsleep mode coupled with
 * 	 peipheral clock gating
 * NOTE: The two voltage regulators can be configured as normal or LP
 * NOTE: The HSI, HSE, PLL, and 1.2 V domain are all disabled
 *
 * regulator: Which regualtor in which mode (check `regulator_mode_t` in pwr.h)
 * flash_mode: 1 to enable flash power-down mode 0 to disable it
 */
uint8_t PWR_enter_stop_on_exit_mode(regulator_mode_t regulator,
				    uint8_t flash_mode, uint8_t entry);

/*
 * Enter Stop-on-exit mode
 * NOTE: See `PWR_enter_stop_mode` for more information
 */
uint8_t PWR_enter_stop_on_exit_mode(regulator_mode_t regulator,
				    uint8_t flash_mode, uint8_t entry);

/*
 * Enter Standby mode
 *
 * NOTE: Standby mode is the lowest power consumption mode
 *
 * NOTE: The 1.2 domain is switched off. All clks are switched off
 * 	 All register states are lost but for the backup domain
 */
uint8_t PWR_enter_standby(uint8_t entry);

/*
 * Enter Standby-on-exit mode
 * NOTE: See `PWR_enter_standby_mode` for more information
 */
uint8_t PWR_enter_standby_on_exit(uint8_t entry);

#endif				// PWR_H
