/*
 * Header for the watchdog timer driver
 * Handles both Independent watchdog timer (IWDG)
 * and Window Watchdog timer (WWDG)
 *
 * Date: Tue May 26 09:25:03 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#ifndef WDG_H
#define WDG_H

#include <stdint.h>
#include <math.h>
#include <drivers/rcc.h>

typedef struct {
	volatile uint32_t *const KR;	// Key register
	volatile uint32_t *const PR;	// Prescaler register
	volatile uint32_t *const RLR;	// Reload register
	volatile uint32_t *const SR;	// Status register
} IWDG_t;

#define IWDG *((IWDG_t *) 0x40003000)

typedef struct {
	volatile uint32_t *const CR;	// Control register
	volatile uint32_t *const CFR;	// Configuration register
	volatile uint32_t *const SR;	// Status register
} WWDG_t;

#define WWDG *((WWDG_t *) 0x40002C00)

#define MAX_RELOAD_TIME 32768.0f
#define MIN_RELOAD_TIME 0.125f

static uint8_t wwdg_reload = 0;
static uint8_t wwdg_initialized = 0;

/*
 * Initialize the Independent Watchdog timer
 *
 * reload_time: The timer reload time is ms
 * NOTE: The reload_time must lie between MAX_RELOAD_TIME and MIN_RELOAD_TIME
 * NOTE: The function doesn't start the IWDG timer
 *
 * NOTE: The application is resposible for initial watchdog configuration
 * (i.e. hardware or software driven). This is configured from the FLASH CFRs
 */
uint8_t IWDG_init(uint32_t reload_time);

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

/*
 * Initialize the Window Watchdog timer
 *
 * reload_time: The timer reload time in ms
 * window_time: After this time, the WWDG can be reloaded.
 * Before it, a reset happens.
 *
 * NOTE: The function doesn't start the WWDG timer
 */
uint8_t WWDG_init(uint32_t reload_time, uint32_t window_time);

/*
 * Start the WWDG timer
 *
 * Return nothing
 */
static inline void WWDG_start(void)
{
	WWDG->CR |= 0x01 << 7;
}

/*
 * Stop the WWDG timer
 *
 * Return nothing
 */
static inline void WWDG_stop(void)
{
	WWDG->CR &= ~(0x01 << 7);
}

/*
 * Pet the WWDG timer
 *
 * NOTE: Any petting outside the window results in a reset.
 *
 * Return 0 upon success and 1 if the WWDG is not initialized
 */
static inline uint8_t WWDG_reload(void)
{
	if (!wwdg_initialized)
		return 1;

	WWDG->CR = wwdg_reload;
	return 0;
}

#endif				// WDG_H
