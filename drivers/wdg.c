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
 * reload_time: The timer reload time is ms
 * NOTE: The reload_time must lie between MAX_RELOAD_TIME and MIN_RELOAD_TIME
 * NOTE: The init function doesn't start the IWDG timer
 *
 * NOTE: The application is resposible for initial watchdog configuration
 * (i.e. hardware or software driven). This is configured from the FLASH CFRs
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t IWDG_init(uint32_t reload_time)
{
	uint32_t prescaler = 0;
	uint32_t prescaler_reg = 0;
	uint32_t reload_value = 0;

	if (reload_time > MAX_RELOAD_TIME || reload_time < MIN_RELOAD_TIME)
		return 1;

	if (reload_time < 512){
		prescaler = 4;
		prescaler_reg = 0;
	}
	else if (reload_time < 1024){
		prescaler = 8;
		prescaler_reg = 1;
	}
	else if (reload_time < 2048){
		prescaler = 16;
		prescaler_reg = 2;
	}
	else if (reload_time < 4096){
		prescaler = 32;
		prescaler_reg = 3;
	}
	else if (reload_time < 8192){
		prescaler = 64;
		prescaler_reg = 4;
	}
	else if (reload_time < 16384){
		prescaler = 128;
		prescaler_reg = 5;
	}
	else {
		prescaler = 256;
		prescaler_reg = 6;
	}

	IWDG->KR = 0x5555;
	IWDG->PR = prescaler_reg;

	reload_value = (reload_time * LSI_FRQ) / (prescaler * 1000) ;
	IWDG->KR = 0x5555;
	IWDG->RLR = reload_value;

	return 0;
}

/*
 * Start the IWDG timer (Action cannot be undone)
 *
 * Return nothing
 */
static inline void IWDG_start(void)
{
	IWDG->KR = 0xCCCC;
}

/*
 * Pet the IWDG timer
 *
 * Return nothing
 */
static inline void IWDG_reload(void)
{
	IWDG->KR = 0xAAAA;
}
