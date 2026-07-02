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
#define ADDR_RCC_AHB1ENR		((volatile uint32_t*)(RCC_BASE+0x30))
// AHB2 peripheral clock enable
#define RCC_AHB2ENR		*((volatile uint32_t*)(RCC_BASE+0x34))
#define ADDR_RCC_AHB2ENR		((volatile uint32_t*)(RCC_BASE+0x34))
// APB1 peripheral clock enable
#define RCC_APB1ENR		*((volatile uint32_t*)(RCC_BASE+0x40))
#define ADDR_RCC_APB1ENR		((volatile uint32_t*)(RCC_BASE+0x40))
// APB2 peripheral clock enable
#define RCC_APB2ENR		*((volatile uint32_t*)(RCC_BASE+0x44))
#define ADDR_RCC_APB2ENR		((volatile uint32_t*)(RCC_BASE+0x44))
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

// Max/min prescaler value of the following PLL divisors/multipliers
#define MAX_M 2
#define MIN_M 1
#define MAX_N 432
#define MIN_N 192
#define MAX_P 84
#define MIN_R 2
#define MAX_R 7
// Max/min output frequency (in Hz) of the following PLL divisors/multipliers
#define MAX_M_FRQ (2*1000000)
#define MIN_M_FRQ (1*1000000)
#define MAX_N_FRQ (432*1000000)
#define MIN_N_FRQ (192*1000000)
#define MAX_P_FRQ (84*1000000)
#define MIN_R_FRQ (2*1000000)
#define MAX_R_FRQ (7*1000000)
#define MAX_Q_FRQ (48*1000000)
// Max RTC output frequency (in Hz)
#define MAX_RTC_FRQ (1000000)
// Max AHB bus prescaler
#define MAX_AHB_PRS 512

// HSI frequency (in Hz)
#define HSI_FRQ 16000000
// HSE board crystal oscillator frequency (in Hz)
#define HSE_FRQ 25000000
// LSE frequency (in Hz)
#define LSE_FRQ 32768
// LSI frequency (in Hz)
#define LSI_FRQ 32000
// MCO pin (for clock out functionality)
typedef enum {
	MCO1, MCO2
} MCO_t;

// All clocks
typedef enum {
	clk_LSI, clk_LSE, clk_HSI, clk_HSE, clk_PLL, clk_PLLI2S, clk_SYSCLK
} clk_t;

// System clock options
typedef enum {
	HSE, HSI, PLL
} sysclk_src_t;

// Bus options
typedef enum {
	AHB, APB1, APB2
} bus_t;

// Peripherals
typedef enum {
	GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOH, CRC, OTGFS,
	TIM1, TIM2, TIM3, TIM4, TIM5, TIM9, TIM10, TIM11,
	WWDG, SPI2, SPI3, USART1, USART2, USART6, I2C1, I2C2,
	SDIO, SPI1, SPI4, DMA1, DMA2, PWR, SYSCFG, I2C3, ADC1
} peripheral_t;

// Peripheral register lookup table
typedef struct {
	volatile uint32_t *enr;
	uint32_t bit_position;
} peripheral_reg_t;

/*
 * Set the clock source for the system clock (SYSCLOCK)
 * Use enum values from `sysclk_src` (Inc/drivers/rcc.h)
 * NOTE: if you wish to use the PLL as the SYSCLOCK, it is advised
 * 		 that you configure it using `RCC_set_PLL()`
 */
uint8_t RCC_set_sysclk_src(sysclk_src_t src);

/* Configure the main PLL.
 * Use enum values from `sysclk_src` (Inc/drivers/rcc.h)
 *
 * NOTE: it's an error if you call this function when the PLL is used as the system clock
 * NOTE: the PLL is turned off for configuration and then restored at the end.
 * NOTE: for HSE clock, you need to input the correct parameters to insure that
 * 		 the output clock frequency is in the valid range.
 *
 * src: clock input to the PLL (either HSE or HSI)
 * M: HSI/HSE clock input division factor.
 * N: VCO output multiplier.
 * P: SYSCLK output division factor.
 * Q: SDIO, RNG, and USB output division factor.
 */
uint8_t RCC_set_PLL(sysclk_src_t src, uint32_t M, uint32_t N, uint32_t P,
		    uint32_t Q);

/*
 * Initializes the PLLI2S
 * NOTE: The main PLL controls the input clock frequency source and the M multiplier
 */
uint8_t RCC_set_PLLI2S(uint32_t R, uint32_t N);

/*
 * Initialize the RTC clock
 * NOTE: RTCCLK shouldn't exceed 100 MHz
 *
 * clk: input clock source (either LSE, LSI, or HSE)
 * prescaler: divisor in case HSE is used as clock input
 */
uint8_t RCC_set_RTC(clk_t clk, uint32_t prescaler);

/*
 * Initialize the SSM (spread spectrum modulation) clock
 * NOTE: This function should be called before the main PLL is enabled or after it's disabled
 */
uint8_t RCC_set_SSM(uint32_t modulation_period, uint32_t inc_step,
		    uint32_t spread_select);

/*
 * Set the prescaler for a specific bus.
 *
 * NOTE: For the APBx buses, the input is the AHB bus clock.
 *		 For the AHB bus, the input is the system clock.
 */
uint8_t RCC_set_bus_prescaler(bus_t bus, uint32_t prescaler);

/*
 * Get the prescaler for a specific bus.
 *
 * NOTE: For the APBx buses, the input is the AHB bus clock.
 *		 For the AHB bus, the input is the system clock.
 * prescaler: A pointer to hold the raw prescaler value
 *
 * Return 0 upon success and 1 otherwise.
 */
uint8_t RCC_get_bus_prescaler(bus_t bus, uint32_t * prescaler);

/*
 * Set the clock prescaler for the timers using the APBx buses
 * NOTE: If the bus prescaler (configured using `RCC_bus_prescaler()`) for the APBx buses is set to 1,
 * 		 the clock used for these timers is just the HCLK (AHB bus clock)
 *
 * prescaler: either 4 or 2.
 *
 * NOTE: In case the APBx bus prescaler is 1, the timer clock is HCLK regardless of this setting.
 */
uint8_t RCC_set_TIM_prescaler(uint32_t prescaler);

/*
 * Enable the main PLL and wait till it's locked
 */
uint8_t RCC_enable_PLL(void);

/*
 * Disable the main PLL and wait till it's unlocked
 * NOTE: You can't disable the PLL if it's used as system clock
 */
uint8_t RCC_disable_PLL(void);

/*
 * Enable the PLLI2S and wait till it's locked
 */
uint8_t RCC_enable_PLLI2S(void);

/*
 * Disable the PLLI2S and wait till it's unlocked
 */
uint8_t RCC_disable_PLLI2S(void);

/*
 * Enable the RTC clock
 */
static inline void RCC_enable_RTC(void)
{
	RCC_BDCR |= 1 << 16;
}

/*
 * Disable the RTC clock
 */
static inline void RCC_disable_RTC(void)
{
	RCC_BDCR &= ~(1 << 16);
}

/*
 * Set and activate an output clock on one of the MCO pins
 * NOTE: Clock selection may generate glitches on MCOx.
 * 		 It is highly recommended to call this function only after
 * 		 reset before enabling the external oscillators and the PLLs.
 *
 * clk: output clock on the MCOx pin
 * mco: output pin (See `enum MCO_t`)
 * prescaler: division factor
 * 			  legal values: 1(pass),2,3,4,5
 */
uint8_t RCC_clockout(clk_t clk, MCO_t mco, uint32_t prescaler);

/*
 * Reset a peripheral (check `enum peripheral_t`)
 * works by setting and clearing the reset bit
 */
uint8_t RCC_reset_peripheral(peripheral_t peripheral);

/*
 * Enable a peripheral (check `enum peripheral_t`)
 */
uint8_t RCC_enable_peripheral(peripheral_t peripheral);

/*
 * Enable a peripheral when the deivce is sleep mode (check `enum peripheral_t`)
 */
uint8_t RCC_enable_LP_peripheral(peripheral_t peripheral);

/*
 * Enable the SSM module.
 * NOTE: This can only be done before the PLL is enabled (before setting the PLLON bit).
 */
uint8_t RCC_enable_SSM(void);

/*
 * Disable the SSM module.
 * NOTE: This can only be done after the PLL is disabled (after clearing the PLLON bit).
 */
uint8_t RCC_disable_SSM(void);

/*
 * Disable a peripheral when the device is sleep mode (check `enum peripheral_t`)
 */
uint8_t RCC_disable_LP_peripheral(peripheral_t peripheral);

/*
 * Disable a peripheral (check `enum peripheral_t`)
 */
uint8_t RCC_disable_peripheral(peripheral_t peripheral);

/*
 * Get the current system clock frequency.
 *
 * freq: A pointer to a uint32_t to store the frequency in.
 */
uint8_t RCC_get_SYSCLK_freq(uint32_t * freq);

/*
 * Get the output frequencies of the main PLL
 * NOTE: A certain clock source can be used as input to the PLL but this doesn't
 * 		 mean that it's stable. This is why there is a check on the XXXRDY bits.
 *
 * Both parameters are pointers to store the return.
 * pfreq: The output frequency after the P divisor (goes to systemclk MUX)
 * qfreq: The output frequency after the Q divisor (goes to the USB_OTG, SDIO, and RNG)
 */
uint8_t RCC_get_PLL_clkout(uint32_t * pfreq, uint32_t * qfreq);

/*
 * Get the output frequency of a specified bus
 *
 * freq: A pointer to a uint32_t to store the return frequency
 * bus: AHB, APB1, or APB2 (check `enum bus_t`)
 */
uint8_t RCC_get_bus_clkout(uint32_t * freq, bus_t bus);

#endif				// RCC_H
