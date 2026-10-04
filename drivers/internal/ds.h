/*
 * Internal private data structures seen by driver APIs
 * Date: Sun Oct 4 05:29:06 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#ifndef DS_H
#define DS_H

/*
 * Peripheral register lookup table
 *
 * This simple DS can be used by peripherals that have multiple copies
 * of the same register for different peripheral enumurations
 * (For example the GPIOx_MODER port mode register x = [A..E and H])
 *
 * On STM32f devices, these registers have an offset pattern that can
 * make for code that avoids conditional statements
 * (See rcc.c and gpio.c for examples)
 */
typedef struct {
	volatile uint32_t *enr;
	uint32_t bit_position;
} peripheral_reg_t;

// Peripherals
typedef enum {
	GPIOA_p, GPIOB_p, GPIOC_p, GPIOD_p, GPIOE_p, GPIOH_p, CRC_p, OTGFS_p,
	TIM1_p, TIM2_p, TIM3_p, TIM4_p, TIM5_p, TIM9_p, TIM10_p, TIM11_p,
	WWDG_p, SPI2_p, SPI3_p, USART1_p, USART2_p, USART6_p, I2C1_p, I2C2_p,
	SDIO_p, SPI1_p, SPI4_p, DMA1_p, DMA2_p, PWR_p, SYSCFG_p, I2C3_p, ADC1_p
} peripheral_t;

#endif // DS_H
