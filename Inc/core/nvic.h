/* 
 * NVIC (Nested Vector Interrupt Controller) header 
 * Date: Thu May 7 12:24:21 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#ifndef NVIC_H
#define NVIC_H

#include <stdint.h>
#include <limits.h>

#define NVIC_BASE 0xE000E100

typedef struct {
	volatile uint32_t ISER[8]; // Interrupt Set Enable Register
	uint32_t reserved1[24];
	volatile uint32_t ICER[8]; // Interrupt Clear Enable Register
	volatile uint32_t ISPR[8]; // Interrupt Set Pending Register
	uint32_t reserved2[24];
	volatile uint32_t ICPR[8]; // Interrupt Clear Pending Register
	volatile uint32_t IABR[8]; // Interrpt Active Bit Register 
	uint32_t reserved3[56];
	volatile uint8_t IPR[240]; // Interrupt Priority Register 
	uint32_t reserved4[644];
	volatile uint32_t STIR[1]; // Software Trigger Interrupt Register
} NVIC_t;

#define NVIC ((NVIC_t *) 0xE000E100)

/* IMPORTANT: Note that all of the APIs do no input checking.
 * This is to make the implementation neater
 * The caller is responsible for ensuring:
 * - IRQn is valid and correct.
 * - Priority levels are correctly encoded.
 */

/*
 * Enable an IRQ line
 *
 * Returns nothing and does no checking
 */
void NVIC_enableIRQ (uint32_t IRQn);

/*
 * Disable an IRQ line
 *
 * Returns nothing and does not checking
 */
void NVIC_disableIRQ (uint32_t IRQn);

/*
 * Set the pending bit of an IRQ line
 *
 * Returns nothing and does no checking
 */
void NVIC_setPending (uint32_t IRQn);

/*
 * Set the priority of an IRQ line
 *
 * NOTE: Write raw priority. Knows nothing about group/sub priorities
 *
 * Returns nothing and does no checking
 */
void NVIC_setPriority (uint32_t IRQn, uint32_t priority);

/*
 * Clear the pending state of an IRQ line
 *
 * Returns nothing and does no checking
 */
void NVIC_clearPending (uint32_t IRQn);

/*
 * Is the current IRQ line pending?
 *
 * Return 0 if it's pending and 1 if it's not
 */
uint8_t NVIC_isPending (uint32_t IRQn);

/*
 * Is the current IRQ line being processed?
 *
 * Return 0 if it's pending and 1 if it's not
 */
uint8_t NVIC_isActive (uint32_t IRQn);

/*
 * Get the current IRQ line priority
 *
 * Return the priority
 */
uint8_t NVIC_getPriority (uint32_t IRQn);

/*
 * Trigger an interrupt on an IRQ line
 *
 * Returns nothing and does no checking
 */
void NVIC_triggerSoftwareInterrupt(uint32_t IRQn);

#endif // NVIC_H
