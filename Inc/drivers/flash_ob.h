/*
 * Flash option bytes header
 * The flash option bytes are configured by the end user
 * depending on the application requirements
 *
 * Related peripherals can use thin wrappers built around
 * implemented APIs
 *
 * Date: Tue Sep 22 07:39:45 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#ifndef FLASH_OB_H
#define FLASH_OB_H

#include <stdint.h>

#define FLASH_BASE 0x40023C00

// Flash write protection
#define FLASH_OB_WPR	(*(volatile uint32_t *) (FLASH_OB_BASE + 0x00))

// Flash key register
#define FLASH_KEYR	(*(volatile uint32_t *) (FLASH_BASE + 0x04))
// Flash option key register
#define FLASH_OPTKEYR	(*(volatile uint32_t *) (FLASH_BASE + 0x08))
// Flash status register
#define FLASH_SR	(*(volatile uint32_t *) (FLASH_BASE + 0x0C))
// Flash control register
#define FLASH_CR	(*(volatile uint32_t *) (FLASH_BASE + 0x10))
// Flash option control register
#define FLASH_OPTCR	(*(volatile uint32_t *) (FLASH_BASE + 0x14))

/*
 * Set hardware watchdog (IWDG enabled by default)
 *
 * Return 0 upon success and 1 otherwise
 */
void OB_set_hardware_WDG(void);

/*
 * Set software watchdog (IWDG disabled by default)
 *
 * Return 0 upon success and 1 otherwise
 */
void OB_set_software_WDG(void);

/*
 * Set the Brown Out Reset voltage level
 *
 * level: 1, 2, 3, or 0 (See `pwr.h`)
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t OB_set_BOR_level(uint8_t level);

/*
 * Generate reset on entering Stop mode (see `pwr.h`)
 */
uint8_t OB_set_stop_reset(void);

/*
 * Generate reset on entering Standby mode (see `pwr.h`)
 */
uint8_t OB_set_standby_reset(void);

#endif				// FLASH_OB_H
