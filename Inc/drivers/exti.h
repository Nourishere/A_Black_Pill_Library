/*
 * External Interrupt (EXTI) controller header
 * Date: Tue May 12 04:00:59 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#ifndef EXTI_H
#define EXTI_H

#include <stdint.h>
#include <drivers/interrupt.h>

#define EXTI_BASE 0x40014000UL

// Interrupt Mask Register
#define EXTI_IMR 	(*(volatile uint32_t *)(EXTI_BASE+0x00U))
// Event Mask Register
#define EXTI_EMR 	(*(volatile uint32_t *)(EXTI_BASE+0x04U))
// Rising Trigger Selection Register
#define EXTI_RTSR 	(*(volatile uint32_t *)(EXTI_BASE+0x08U))
// Falling Trigger Selection Register
#define EXTI_FTSR 	(*(volatile uint32_t *)(EXTI_BASE+0x0CU))
// Software Interrupt Event Register
#define EXTI_SWIER 	(*(volatile uint32_t *)(EXTI_BASE+0x10U))
// Pending Register
#define EXTI_PR		(*(volatile uint32_t *)(EXTI_BASE+0x14U))

/*
 * Note that lines 20-19 are not implemented for the current device
 */
typedef enum {
	EXTI_0, EXTI_1, EXTI_2, EXTI_3, EXTI_4, EXTI_5, EXTI_6, EXTI_7, EXTI_8,
	    EXTI_9,
	EXTI_10, EXTI_11, EXTI_12, EXTI_13, EXTI_14, EXTI_15, EXTI_16, EXTI_17,
	EXTI_18, EXTI_19, EXTI_20, EXTI_21, EXTI_22
} EXTIn_t;

typedef enum {
	rising, falling, both
} edge_t;

/*
 * Initialize an external interrupt line (see enum `EXTIn_t`)
 * NOTE: An external voltage switch (rise, fall, or both)
 * 		triggers an interrupt on that line.
 *
 * line: EXTI line to set
 * edge: (use enum `edge_t`)
 * priority: Interrupt priority
 */
uint8_t EXTI_set_line(EXTIn_t line, edge_t edge, uint8_t priority);

/*
 * Initialize an external interrupt event (see enum `EXTIn_t`)
 * NOTE: An external voltage switch (rise, fall, or both)
 * 		triggers a special event (see enum `IRQn_t`).
 *
 * line: EXTI line to set
 * edge: (use enum `edge_t`)
 * priority: Interrupt priority
 */
uint8_t EXTI_set_event(EXTIn_t line, edge_t edge, uint8_t priority);

/*
 * Enable IRQ requests on an EXTI line
 * When triggered, the IRQ line is sent to the NVIC
 *
 * line: EXTI line number to enable
 *
 */
uint8_t EXTI_enable_line(EXTIn_t line);

/*
 * Enable event requests on an EXTI line
 *
 * line: EXTI line number to enable
 */
uint8_t EXTI_enable_event(EXTIn_t line);

/*
 * Disable an interrupt line
 *
 * line: EXTI line number to disable
 */
uint8_t EXTI_disable_line(EXTIn_t line);

/*
 * Disable an interrupt event
 *
 * line: EXTI line number to disable
 */
uint8_t EXTI_disable_event(EXTIn_t line);

/*
 * Trigger a software interrupt on an EXTI line
 */
uint8_t EXTI_trigger_interrupt(EXTIn_t line);

/*
 * Clear pending state of en EXTI line
 */
uint8_t EXTI_clear_pending(EXTIn_t line);
#endif				//EXTI_H
