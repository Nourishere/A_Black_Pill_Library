/*
 * Flash option bytes source
 * The flash option bytes are configured by the end user
 * depending on the application requirements
 *
 * Date: Tue Sep 22 07:39:45 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#include <drivers/flash_ob.h>

static inline void OB_unlock(void);

/*
 * Set hardware watchdog (IWDG enabled by default)
 *
 * NOTE: A system reset is required for this to take effect
 */
void OB_set_hardware_WDG(void)
{
	OB_unlock();
	// Check no flash operation is running
	while ((FLASH_SR >> 16) & 0x1) {
		// TODO: Add some timing mechanism
	}

	if ((FLASH_OPTCR >> 5) & 0x1)
		FLASH_OPTCR &= ~(1 << 5);

	// Set the option start bit in the OPTCR register
	FLASH_OPTCR |= 1 << 0x1;

	// Wait for BSY bit to be cleared
	while ((FLASH_SR >> 16) & 0x1) {
		// TODO: Add some timing mechanism
	}
}

/*
 * Set software watchdog (IWDG disabled by default)
 */
void OB_set_software_WDG(void)
{
	OB_unlock();
	// Check no flash operation is running
	while ((FLASH_SR >> 16) & 0x1) {
		// TODO: Add some timing mechanism
	}

	FLASH_OPTCR |= 1 << 5;

	// Set the option start bit in the OPTCR register
	FLASH_OPTCR |= 1 << 0x1;

	// Wait for BSY bit to be cleared
	while ((FLASH_SR >> 16) & 0x1) {
		// TODO: Add some timing mechanism
	}
}

/*
 * Set the Brown Out Reset voltage level
 *
 * level: 1, 2, 3, or 0 (See `pwr.h`)
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t OB_set_BOR_level(uint8_t level)
{
	if (level > 3)
		return 1;

	OB_unlock();
	// Check no flash operation is running
	while ((FLASH_SR >> 16) & 0x1) {
		// TODO: Add some timing mechanism
	}

	FLASH_OPTCR &= ~(0x2 << 2);
	FLASH_OPTCR |= ((3 - level) << 2);

	// Set the option start bit in the OPTCR register
	FLASH_OPTCR |= 1 << 0x1;

	// Wait for BSY bit to be cleared
	while ((FLASH_SR >> 16) & 0x1) {
		// TODO: Add some timing mechanism
	}

	return 0;
}

/*
 * Sequence to unlock the user option section for writing
 */
static inline void OB_unlock(void)
{
	FLASH_OPTKEYR = 0x08192A3B;
	FLASH_OPTKEYR = 0x4C5D6E7F;

	FLASH_OPTCR &= ~(0x01U);
}
