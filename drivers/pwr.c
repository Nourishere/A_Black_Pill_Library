/*
 * Power Controller
 * Date: Tue Sep 22 03:34:45 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#include <drivers/pwr.h>

/*
 * Set up the regulator's scale
 *
 * CAUTION: This function toggles the PLL clock as part of configuration
 *	    This function will return an error if the PLL is currently
 *	    used as system clock
 *
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
 */
uint8_t PWR_set_regulator_scale(uint8_t scale)
{
	sysclk_src_t sysclk;
	if (scale > 3 || scale < 2)
		return 1;
	if (RCC_get_SYSCLK(&sysclk))
		return 1;
	// Can't turn off the PLL if it is used as system clk
	if (sysclk == PLL)
		return 1;

	// Get the state of the PLL (on or off)
	int pll_state = RCC_get_PLL_state();

	// Turn the PLL off if it is on
	if (pll_state)
		if (RCC_disable_PLL())
			return 1;

	// Set the regulator output scale
	PWR_CR &= ~(0x3U << 14);
	if (scale == 2)
		PWR_CR |= 0x2U << 14;
	else
		PWR_CR |= 0x1U << 14;

	// Turn the PLL back on if it was originally on
	if (pll_state) {
		if (RCC_enable_PLL()) {
			PWR_CR &= ~(0x3U << 14);
			return 1;
		}
	}

	return 0;
}

/*
 * Set the PVD threshold value
 *
 * threshold: The voltage threshold detected by the PVD
 * 	      Values are from 2.2 - 2.9 (0.1 V increment)
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t PWR_set_PVD(float threshold)
{
	uint32_t hundredths = (uint32_t) (threshold * 100) % 10;

	if (threshold < 2.2f || threshold > 2.9f || hundredths != 0)
		return 1;

	// A neat way to write to the register without having to
	// go through a lot of conditions
	uint8_t val = (uint8_t) ((threshold - 2.0f) * 10.0f) - 2;

	PWR_CR &= ~(0x7 << 5);
	PWR_CR |= val << 5;

	return 0;
}

/*
 * Enter Sleep mode
 *
 * entry: 0 for event entry (issue a WFE instruction)
 * 	  1 for interrupt entry (issue a WFI instruction)
 *
 * NOTE: Waiting on an event also waits on interrupts
 *
 * Return 0 after successful wakeup and 1 otherwise
 */
uint8_t PWR_enter_sleep_mode(uint8_t entry)
{
	if (entry > 1)
		return 1;

	// Disable Deep Sleep by enabling sleep
	SCB_enable_sleep();

	if (entry)
		SCB_enter_sleep_on_interrupt();
	else
		SCB_enter_sleep_on_event();

	return 0;
}

/*
 * Enter Sleep-on-exit mode
 *
 * NOTE: See `PWR_enter_sleep_mode` for more information
 */
uint8_t PWR_enter_sleep_on_exit_mode(uint8_t entry)
{
	// Enable Sleep on Exit
	SCB_enable_sleeponexit();

	return PWR_enter_sleep_mode(entry);
}

/*
 * Enter Stop mode
 *
 * NOTE: Stop mode is M4 deepsleep mode coupled with
 * 	 peipheral clock gating
 * NOTE: The two voltage regulators can be configured as normal or LP
 * NOTE: The HSI, HSE, PLL, and 1.2 V domain are all disabled
 * NOTE: It is an error if you disable the flash power-down mode
 * 	 when the regulator is set in LP mode (main_lp or low_power_lp)
 *
 * regulator: Which regualtor in which mode (check `regulator_mode_t` in pwr.h)
 * flash_mode: 1 to enable flash power-down mode 0 to disable it
 *
 * Return 0 after successful wakeup and 1 otherwise
 */
uint8_t PWR_enter_stop_mode(regulator_mode_t regulator,
			    uint8_t flash_mode, uint8_t entry)
{
	if (regulator > low_power_lp)
		return 1;
	if (flash_mode > 1)
		return 1;
	if ((regulator == main_lp || regulator == low_power_lp)
	    && (!flash_mode))
		return 1;
	// Clear regulator bits
	PWR_CR &= ~((1 << 0) | (1 << 10) | (1 << 11));

	// Set up regulator
	switch (regulator) {
	case (main):
		// Nothing here
		break;
	case (main_lp):
		PWR_CR |= 1 << 11;
		break;
	case (low_power):
		PWR_CR |= 1 << 0;
		PWR_CR &= ~(1 << 10);
		break;
	case (low_power_lp):
		PWR_CR |= 1 << 0;
		PWR_CR |= 1 << 10;
		break;
	default:
		return 1;
	}

	// Set up flash power mode
	if (flash_mode)
		PWR_CR |= 1 << 9;
	else
		PWR_CR &= ~(1 << 9);

	// Enable Deep Sleep mode
	SCB_enable_deepsleep();

	// Configure Stop mode
	PWR_CR &= ~(1 << 1);

	// Clear all pending bits
	NVIC_clear_all_pending();
	EXTI_clear_all_pending();

	if (entry)
		SCB_enter_sleep_on_interrupt();
	else
		SCB_enter_sleep_on_event();

	return 0;
}

/*
 * Enter Stop-on-exit mode
 * NOTE: See `PWR_enter_stop_mode` for more information
 */
uint8_t PWR_enter_stop_on_exit_mode(regulator_mode_t regulator,
				    uint8_t flash_mode, uint8_t entry)
{
	// Enable Sleep on Exit
	SCB_enable_sleeponexit();

	// Enter Sleep mode
	return PWR_enter_stop_mode(regulator, flash_mode, entry);
}

/*
 * Enter Standby mode
 *
 * NOTE: Standby mode is the lowest power consumption mode
 *
 * NOTE: The 1.2 domain is switched off. All clks are switched off
 * 	 All register states are lost but for the backup domain
 *
 * Return 0 after successful wakeup and 1 otherwise
 */
uint8_t PWR_enter_standby_mode(uint8_t entry)
{
	if (entry > 1)
		return 1;

	// Enable Deep Sleep
	SCB_enable_deepsleep();

	// Configure Standby mode
	PWR_CR |= 1 << 1;

	// Clear all pending bits
	NVIC_clear_all_pending();
	EXTI_clear_all_pending();

	// Clear the wakeup flag
	PWR_CR |= 1 << 2;

	if (entry)
		SCB_enter_sleep_on_interrupt();
	else
		SCB_enter_sleep_on_event();

	return 0;

}

/*
 * Enter Standby-on-exit mode
 * NOTE: See `PWR_enter_standby_mode` for more information
 */
uint8_t PWR_enter_standby_on_exit_mode(uint8_t entry)
{
	// Enable Sleep on Exit
	SCB_enable_sleeponexit();

	// Enter Standby mode
	return PWR_enter_standby_mode(entry);
}
