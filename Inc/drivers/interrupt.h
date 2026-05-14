/*
 * STM32F401 specific interrupt header
 * Written with portability in mind
 * Date: Fri May 8 08:10:20 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>
#include <core/scb.h>
#include <core/nvic.h>
#include <core/cpu.h>

// Total priority bits for the device
#define INTERRUPT_PRIO_BITS 4

/*
 * All IRQ lines on the device with aliases
 */
typedef enum {
	WWDG,
	EXTI16 = 1, PVD = 1, EXTI21 = 2, TAMP_STAMP = 2, EXTI22 = 3,
	RTC_WKUP = 3, FLASH, RCC, EXTI0, EXTI1, EXTI2, EXTI3, EXTI4,
	DMA1_Stream0, DMA1_Stream1, DMA1_Stream2, DMA1_Stream3,
	DMA1_Stream4, DMA1_Stream5, DMA1_Stream6, ADC,
	// Reserved IRQ lines here
	EXTI9_5 = 23, EXTI9 = 23, EXTI8 = 23, EXTI7 = 23, EXTI6 = 23, EXTI5 =
	    23,
	TIM1_BRK_TIM9, TIM1_UP_TIM10, TIM1_TRG_COM = 26, TIM11 = 26, TIM1_CC,
	TIM2, TIM3, TIM4, I2C1_EV, I2C1_ER, I2C2_EV, I2C2_ER, SPI1, SPI2,
	USART1, USART2,
	// Reserved IRQ lines here
	EXTI15_10 = 40, EXTI15 = 40, EXTI14 = 40, EXTI13 = 40, EXTI12 = 40,
	EXTI11 = 40, EXTI10 = 40, EXTI17 = 41, RTC_Alarm = 41, EXTI18 = 42,
	OTG_FS_WKUP = 42,
	// Reserved IRQ lines here
	DMA1_Stream7 = 47,
	// Reserved IRQ line here
	SDIO = 49, TIM5, SPI3,
	// Reserved IRQ line here
	DMA2_Stream0 = 56, DMA2_Stream1, DMA2_Stream2, DMA2_Stream3,
	DMA2_Stream4,
	// Reserved IRQ line here
	OTG_FS = 67, DMA2_Stream5, DMA2_Stream6, DMA2_Stream7, USART6,
	I2C3_EV, I2C3_ER,
	// Reserved IRQ line here
	FPU = 81,
	// Reserved IRQ line here
	PI4 = 84
} IRQn_t;

/*
 * Initialize interrupts by doing the following:
 * - Setting the preempt/subgroup.
 * - Disabling interrupts completely.
 * - Clearning priority and pending states.
 *
 * NOTE: This function gives you the possibility to either use or ignore sub
 * 		priorities. It should be called once at initialization time.
 *
 * groupbits: The number of group priority bits.
 * subbits: The number of sub priority bits.
 *
 * NOTE: if you wish NOT to use any sub priority, just
 * 		set sublevels to 0 and grouplevels to INTERRUPT_PRIO_BITS.
 */
uint8_t interrupt_init(uint8_t groupbits);

/*
 * Set the priority of an interrupt line
 *
 * NOTE: The interrupt priority you give is the full 8-bit priority
 *
 * priority: The priority you wish to give out
 */
uint8_t interrupt_set_priority(IRQn_t line, uint8_t priority);

/*
 * Fetch the current priority on an IRQ line
 *
 * Use a pointer for return
 */
uint8_t interrupt_get_priority(IRQn_t line, uint8_t * priority);

/*
 * Set the pending state for an IRQ line
 */
uint8_t interrupt_set_pending(IRQn_t line);

/*
 * Clear the pending state for an IRQ line
 */
uint8_t interrupt_clear_pending(IRQn_t line);

/*
 * Is the IRQ line pending service?
 * Return 1 if it is and 0 if is not
 */
uint8_t interrupt_is_pending(IRQn_t line);

/*
 * Is the IRQ line being served?
 * Return 1 if it is and 0 if is not
 */
uint8_t interrupt_is_active(IRQn_t line);
/*
 * Enable an IRQ line
 */
uint8_t interrupt_enable_IRQ(IRQn_t line);

/*
 * Disable an IRQ line
 */
uint8_t interrupt_disable_IRQ(IRQn_t line);

#endif				// INTERRUPT_H
