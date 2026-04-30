/* 
 * RCC (Reset and clock control) module header.
 * Date: Sun 22 Feb 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#ifndef RCC_H
#define RCC_H

#include <stdint.h>

#define RCC_BASE 0x40023800
// Control register
#define RCC_CR			*((volatile uint32_t*)(RCC_BASE+0x00))
// PLL configuration register
#define RCC_PLLCFGR		*((volatile uint32_t*)(RCC_BASE+0x04))
// Clock configuration register
#define RCC_CFGR		*((volatile uint32_t*)(RCC_BASE+0x08))
// Clock interrupt register
#define RCC_CIR			*((volatile uint32_t*)(RCC_BASE+0x0C))
// AHB1 peripheral reset register
#define RCC_AHB1RSTR	*((volatile uint32_t*)(RCC_BASE+0x10))
// AHB2 peripheral reset register
#define RCC_AHB2RSTR	*((volatile uint32_t*)(RCC_BASE+0x14))
// APB1 peripheral reset register
#define RCC_APB1RSTR	*((volatile uint32_t*)(RCC_BASE+0x20))
// APB2 peripheral reset register
#define RCC_APB2RSTR	*((volatile uint32_t*)(RCC_BASE+0x24))
// AHB1 peripheral clock enable
#define RCC_AHB1ENR		*((volatile uint32_t*)(RCC_BASE+0x30))
// AHB2 peripheral clock enable
#define RCC_AHB2ENR		*((volatile uint32_t*)(RCC_BASE+0x34))
// APB1 peripheral clock enable
#define RCC_APB1ENR		*((volatile uint32_t*)(RCC_BASE+0x40))
// APB2 peripheral clock enable
#define RCC_APB2ENR		*((volatile uint32_t*)(RCC_BASE+0x44))
// AHB1 peripheral clock enable in low power mode
#define RCC_AHB1LPENR	*((volatile uint32_t*)(RCC_BASE+0x50))
// AHB2 peripheral clock enable in low power mode
#define RCC_AHB2LPENR	*((volatile uint32_t*)(RCC_BASE+0x54))
// APB1 peripheral clock enable in low power mode
#define RCC_APB1LPENR	*((volatile uint32_t*)(RCC_BASE+0x60))
// APB2 peripheral clock enable in low power mode
#define RCC_APB2LPENR	*((volatile uint32_t*)(RCC_BASE+0x64))
// Backup domain control register
#define RCC_BDCR		*((volatile uint32_t*)(RCC_BASE+0x70))
// Clock control & status register
#define RCC_CSR			*((volatile uint32_t*)(RCC_BASE+0x74))
// Spread specturm clock generation register
#define RCC_SSCGR		*((volatile uint32_t*)(RCC_BASE+0x80))
// PLLI2S configuration register
#define RCC_PLLI2SCFGR	*((volatile uint32_t*)(RCC_BASE+0x84))
// Dedicated clocks configuration register 
#define RCC_DCKCFGR		*((volatile uint32_t*)(RCC_BASE+0x8C))

// Max/min output frequency (in MHz) of the following PLL divisors/multipliers
#define MAX_M 2
#define MIN_M 1
#define MAX_N 432
#define MIN_N 192
#define MAX_P 84

// HSI frequency (in MHz)
#define HSI_FRQ 16

// MCO pin (for clock out functionality)
typedef enum{
	MCO1,MCO2
}MCO_t;

// All clocks
typedef enum{
	clk_LSI,clk_LSE,clk_HSI,clk_HSE
	,clk_PLL,clk_PLLI2S,clk_SYSCLK
}clk_t;

// System clock options
typedef enum{
	HSE,HSI,PLL
}sysclk_src_t;

// Bus options
typedef enum{
	AHB1,AHB2,APB1,APB2
}bus_t;

// Peripherals
typedef enum{
	GPIOA,GPIOB,GPIOC,GPIOD,GPIOE,GPIOH,CRC,OTGFS,
	TIM1,TIM2,TIM3,TIM4,TIM5,TIM9,TIM10,TIM11,
	WWDG,SPI2,SPI3,USART1,USART2,USART6,I2C1,I2C2,
	SDIO,SPI1,SPI4,DMA1,DMA2,PWR,SYSCFG,I2C3,ADC1
}peripheral_t;

uint8_t RCC_set_clksrc(sysclk_src_t src);
uint8_t RCC_set_PLL(sysclk_src_t src, uint32_t M, uint32_t N, uint32_t P, uint32_t Q);
uint8_t RCC_clockout(clk_t clk, MCO_t mco, uint32_t prescaler);
#endif // RCC_H
