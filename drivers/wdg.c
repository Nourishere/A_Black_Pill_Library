/*
 * Watchdog timer driver
 * Handles both of Independent watchdog timer (IWDG)
 * and Window Watchdog timer (WWDG)
 *
 * Date: Tue May 26 09:25:03 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#include <drivers/wdg.h>

/*
 * Initialize the Independent Watchdog timer
 *
 * reload_time: The timer reload time is ms (the IWDG will need petting every reload_time ms)
 * NOTE: The reload_time must lie between MAX_RELOAD_TIME and MIN_RELOAD_TIME
 * NOTE: The init function doesn't start the IWDG timer
 *
 * NOTE: The application is resposible for the initial watchdog configuration
 * (i.e. hardware or software driven). This is configured from the FLASH CFRs
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t IWDG_init(float reload_time)
{
	// Start with the lowest prescaler value and go down
	uint32_t prescaler = 4;
	uint32_t prescaler_register;
	uint32_t reload;
	if (reload_time > MAX_RELOAD_TIME || reload_time < MIN_RELOAD_TIME)
		return 1;
	// Keep increasing the prescaler till the reload value is valid
	while (prescaler <= 256) {
		reload =
		    ((reload_time * (float)LSI_FRQ) / (prescaler * 1000U)) - 1;
		if (reload < 4096)
			break;
		prescaler *= 2;
	}
	if (prescaler > 256)
		return 1;
	while (prescaler > 4) {
		prescaler /= 2;
		prescaler_register++;
	}
	IWDG->KR = 0x5555;
	IWDG->PR = prescaler_register;
	IWDG->RLR = reload;

	return 0;
}

/*
 * Initialize the Window Watchdog timer
 *
 * reload_time: The timer reload time in ms
 * window_time: After this time, the WWDG can be reloaded.
 * Before it, a reset happens.
 * NOTE: The APB1 bus must be configured before this function is called
 *
 * NOTE: The function doesn't start the WWDG timer and stops it in case it is started
 *
 * Return 0 upon success and 1 otherwise
 */

uint8_t WWDG_init(uint32_t reload_time, uint32_t window_time)
{
	uint32_t apb1_clk;
	uint8_t found = 0;
	float tick;
	float downcounter;
	float windowcounter;
	uint32_t timer_base;

	// Disable if enabled
	if ((WWDG->CR >> 7) & 0x01)
		WWDG->CR &= ~(0x01 << 7);

	if (window_time > reload_time)
		return 1;
	if (RCC_get_bus_clkout(&apb1_clk, APB1))
		return 1;
	// apb1_period is in ms and apb1_clk is in Hz
	float apb1_period = 1000.0f / ((float)apb1_clk);

	// reload time = apb period * 4096 * 2^ timer base *  7bit counter
	float exp_min = 4096.0f * 8.0f * 64.0f;
	float exp_max = 4096.0f;
	float apb1_max_timeout = apb1_period * exp_max;
	float apb1_min_timeout = apb1_period * exp_min;

	if (reload_time > apb1_max_timeout || reload_time < apb1_min_timeout)
		return 1;

	for (timer_base = 0; timer_base < 4; timer_base++) {
		tick = apb1_period * 4096.0f * (float)(1 << timer_base);
		downcounter = 0x40 | ((uint32_t) (reload_time / tick) - 1);
		if (downcounter > 0x7F || downcounter < 0)
			continue;
		windowcounter = 0x40 | ((uint32_t) (window_time / tick) + 1);
		if (windowcounter > 0x7F)
			continue;
		found = 1;
		break;
	}
	if (!found)
		return 1;

	WWDG->CR &= ~0x7F;
	WWDG->CR |= ((uint32_t) downcounter & 0x7F);

	WWDG->CFR &= ~(0x03 << 7);
	WWDG->CFR |= (timer_base & 0x03) << 7;

	WWDG->CFR &= ~0x7F;
	WWDG->CFR |= (uint32_t) windowcounter & 0x7F;

	wwdg_reload = (uint32_t) downcounter;
	wwdg_initialized = 1;
	return 0;
}
