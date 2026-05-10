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
void NVIC_enableIRQ (uint32_t IRQn) 
{
	NVIC->ISER[IRQn/32] |= (1U << (IRQn%32));
}

/*
 * Disable an IRQ line
 *
 * Return nothing
 */
void NVIC_disableIRQ (uint32_t IRQn)
{
	NVIC->ICER[IRQn/32] |= (1U << (IRQn%32));
}

/*
 * Set the pending state of an IRQ line
 *
 * Return nothing
 */
void NVIC_setPending (uint32_t IRQn)
{
	NVIC->ISPR[IRQn/32] |= (1U << (IRQn%32));
}

/*
 * Set the priority of an IRQ line
 *
 * NOTE: The priority set is the raw 8-bit priority
 * 		 The caller should probably encode it first 
 *
 * Return nothing
 */
void NVIC_setPriority (uint32_t IRQn, uint32_t priority)
{
	NVIC->IPR[IRQn] = (uint8_t) priority;
}

/*
 * Clear the pending state of an IRQ line
 *
 * Return nothing
 */
void NVIC_clearPending (uint32_t IRQn)
{
	NVIC->ICPR[IRQn/32] |= (1U << (IRQn % 32));
}

/*
 * Is the current IRQ line pending?
 *
 * Return 1 if the IRQ line is pending and 0 otherwise
 */
uint8_t NVIC_isPending (uint32_t IRQn)
{
	return (NVIC->ISPR[IRQn/32] >> (IRQn % 32)) & 0x01U;
}

/*
 * Is the current IRQ being processed?
 *
 * Return 1 if the IRQ line is active and 0 otherwise 
 */
uint8_t NVIC_isActive (uint32_t IRQn)
{
	return (NVIC->IABR[IRQn/32] >> (IRQn % 32)) & 0x01U;
}

/*
 * Get the priority of an IRQ line
 * 
 * NOTE: The priority returned is the raw 8-bit priority
 * 		 The caller should probably decode it after
 *
 * Return the priority level of the IRQ line
 */
uint8_t NVIC_getPriority (uint32_t IRQn)
{
	return NVIC->IPR[IRQn];
}

/*
 * Trigger an interrupt on an IRQ line
 *
 * Return nothing
 */
void NVIC_triggerSoftwareInterrupt(uint32_t IRQn)
{
	NVIC->STIR[1] = IRQn;
}
