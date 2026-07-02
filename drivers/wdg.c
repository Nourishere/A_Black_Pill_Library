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
uint8_t IWDG_init(uint32_t reload_time)
{
	uint32_t prescaler = 0;
	uint32_t prescaler_reg = 0;
	uint32_t reload_value = 0;

	// Wait if there are any ongoing updates to the prescaler or the reload value
	while (IWDG->SR & 0x01) ;
	while ((IWDG->SR >> 1) & 0x01) ;

	if (reload_time > MAX_RELOAD_TIME || reload_time < MIN_RELOAD_TIME)
		return 1;

	if (reload_time <= 512) {
		prescaler = 4;
		prescaler_reg = 0;
	} else if (reload_time <= 1024) {
		prescaler = 8;
		prescaler_reg = 1;
	} else if (reload_time <= 2048) {
		prescaler = 16;
		prescaler_reg = 2;
	} else if (reload_time <= 4096) {
		prescaler = 32;
		prescaler_reg = 3;
	} else if (reload_time <= 8192) {
		prescaler = 64;
		prescaler_reg = 4;
	} else if (reload_time <= 16384) {
		prescaler = 128;
		prescaler_reg = 5;
	} else {
		prescaler = 256;
		prescaler_reg = 6;
	}

	IWDG->KR = 0x5555;
	IWDG->PR = prescaler_reg;

	reload_value = (reload_time * LSI_FRQ) / (prescaler * 1000.0);
	IWDG->KR = 0x5555;
	IWDG->RLR = 0x7FF & reload_value;

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
	double tick;
	double downcounter;
	double windowcounter;
	uint32_t timer_base;

	// Disable if enabled
	if ((WWDG->CR >> 7) & 0x01)
		WWDG->CR &= ~(0x01 << 7);

	if (window_time > reload_time)
		return 1;
	if (RCC_get_bus_clkout(&apb1_clk, APB1))
		return 1;
	// apb1_period is in ms and apb1_clk is in Hz
	double apb1_period = 1000.0 / ((double)apb1_clk);

	// reload time = apb period * 4096 * 2^ timer base *  7bit counter
	double exp_min = 4096.0 * 8.0 * 64.0;
	double exp_max = 4096.0;
	double apb1_max_timeout = apb1_period * exp_max;
	double apb1_min_timeout = apb1_period * exp_min;

	if (reload_time > apb1_max_timeout || reload_time < apb1_min_timeout)
		return 1;

	for (timer_base = 0; timer_base < 4; timer_base++) {
		tick = apb1_period * 4096.0 * (double)(1 << timer_base);
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
