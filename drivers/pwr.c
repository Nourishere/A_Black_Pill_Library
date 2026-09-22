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
 *
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
		PWR_CR |= 0x1U << 14;
	else
		PWR_CR |= 0x2U << 14;

	// Turn the PLL back on if it was originally on
	if (pll_state)
		if (RCC_enable_PLL())
			return 1;

	return 0;
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
uint8_t PWR_set_PVD(float threshold)
{
	uint32_t hundredths = (uint32_t) (threshold * 100) % 10;

	if (threshold < 2.2f || threshold > 2.9f || hundredths != 0)
		return 1;

	// A neat way to write to the register without having to
	// go through a lot of conditions
	uint8_t val = (uint8_t) ((threshold - 2.0f) * 10.0f) - 2;

	PWR_CR &= ~(0x3 << 5);
	PWR_CR |= val << 5;

	return 0;
}
