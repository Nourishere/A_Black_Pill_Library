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
 */
static inline void IWDG_start(void);

/*
 * Pet the IWDG timer
 */
static inline void IWDG_reload(void);

#endif				// WDG_H
