/* 
 * NVIC (Nested Vector Interrupt Controller) source
 * Date: Thu May 7 12:24:21 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#include <core/nvic.h>

/*
 * Enable an IRQ line 
 *
 * Return nothing
 */
void NVIC_enable_IRQ(uint32_t IRQn)
{
	NVIC->ISER[IRQn / 32] |= (1U << (IRQn % 32));
}

/*
 * Disable an IRQ line
 *
 * Return nothing
 */
void NVIC_disable_IRQ(uint32_t IRQn)
{
	NVIC->ICER[IRQn / 32] |= (1U << (IRQn % 32));
}

/*
 * Set the pending state of an IRQ line
 *
 * Return nothing
 */
void NVIC_set_pending(uint32_t IRQn)
{
	NVIC->ISPR[IRQn / 32] |= (1U << (IRQn % 32));
}

/*
 * Set the priority of an IRQ line
 *
 * NOTE: The priority set is the raw 8-bit priority
 * 		 The caller should probably encode it first 
 *
 * Return nothing
 */
void NVIC_set_priority(uint32_t IRQn, uint32_t priority)
{
	NVIC->IPR[IRQn] = (uint8_t) priority;
}

/*
 * Clear the pending state of an IRQ line
 *
 * Return nothing
 */
void NVIC_clear_pending(uint32_t IRQn)
{
	NVIC->ICPR[IRQn / 32] |= (1U << (IRQn % 32));
}

/*
 * Is the current IRQ line pending?
 *
 * Return 1 if the IRQ line is pending and 0 otherwise
 */
uint8_t NVIC_is_pending(uint32_t IRQn)
{
	return (NVIC->ISPR[IRQn / 32] >> (IRQn % 32)) & 0x01U;
}

/*
 * Is the current IRQ being processed?
 *
 * Return 1 if the IRQ line is active and 0 otherwise 
 */
uint8_t NVIC_is_active(uint32_t IRQn)
{
	return (NVIC->IABR[IRQn / 32] >> (IRQn % 32)) & 0x01U;
}

/*
 * Get the priority of an IRQ line
 * 
 * NOTE: The priority returned is the raw 8-bit priority
 * 		 The caller should probably decode it after
 *
 * Return the priority level of the IRQ line
 */
uint8_t NVIC_get_priority(uint32_t IRQn)
{
	return NVIC->IPR[IRQn];
}

/*
 * Trigger an interrupt on an IRQ line
 *
 * Return nothing
 */
void NVIC_trigger_software_interrupt(uint32_t IRQn)
{
	NVIC->STIR[1] = IRQn;
}

/*
 * Disable all IRQ lines
 */
void NVIC_disable_all(void)
{
	for (uint8_t i = 0; i < 8; i++)
		NVIC->ICER[i] = 0xFFFFFFFFU;
}

/*
 * Clear all the pending states
 */
void NVIC_clear_all_pending(void)
{
	for (uint8_t i = 0; i < 240; i++)
		NVIC->ICPR[i] = 0xFFFFFFFFU;
}

/*
 * Reset all priorities (clear them)
 */
void NVIC_reset_all_priorities(void)
{
	for (uint8_t i = 0; i < 8; i++)
		NVIC->IPR[i] = 0U;
}
