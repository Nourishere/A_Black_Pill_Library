/* 
 * RCC (Reset and clock control) module source
 * Date: Sun Apr 19 02:36:35 PM EET 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#include <stdint.h>
#include <limits.h>
#include <../Inc/drivers/rcc.h>

static void RCC_write_PLL_params(uint32_t M, uint32_t N, uint32_t P, uint32_t Q);
static uint8_t RCC_con_peripheral(peripheral_t peripheral, uint32_t state);

/*
 * Set the clock source for the system clock (SYSCLOCK)
 * NOTE: if you wish to use the PLL as the SYSCLOCK, it is adviced
 * 		 that you configure it using `RCC_set_PLL()`
 *
 * src: SYSCLK main input. Use enum values from `sysclk_src` (Inc/drivers/rcc.h)
 *
 * Return 0 upon success and 1 otherwise.
 */
uint8_t RCC_set_clksrc(sysclk_src_t src){
	switch(src){
		case(HSE):
			RCC_CR |= (1 << 16);
			// wait till ready
			while(!(RCC_CR & (1 << 17))){
				// better add a countdown here	
			}

			// select the clock
			RCC_CFGR &= ~(0x3<<0);
			RCC_CFGR |= (0x1<<0);
			while(((RCC_CFGR >> 2) & (0x03)) != 0x01){
				// better add a countdown here	
			}
			break;
		case(HSI):
			RCC_CR |= (1 << 0);
			while(!(RCC_CR & (1 << 1))){
				// better add a countdown here	
			}

			// select the clock
			RCC_CFGR &= ~(0x3<<0);
			RCC_CFGR |= (0x0<<0);
			while(((RCC_CFGR >> 2) & (0x03)) != 0x00){
				// better add a countdown here	
			}
			break;
		// PLL should be configured (check `RCC_set_PLL`)
		case(PLL):
			RCC_CR |= (1 << 24);
			while(!(RCC_CR & (1 << 25))){
				// better add a countdown here	
			}

			// select the clock
			RCC_CFGR &= ~(0x3<<0);
			RCC_CFGR |= (0x1<<1);
			while(((RCC_CFGR >> 2) & (0x03)) != 0x1<<1){
				// better add a countdown here	
			}
			break;
		default:
			return 1;
	}
	return 0; 
}

/*
 * Configure the main PLL.
 * Use enum values from `sysclk_src` (Inc/drivers/rcc.h)
 * NOTE: the PLL is turned off for configuration and then restored at the end.
 * NOTE: for HSE clock, you need to input the correct parameters to insure that 
 * 		 the output clock frequency is in the valid range.
 *
 * src: clock input to the PLL (either HSE or HSI)
 * M: HSI/HSE clock input division factor. 
 * N: VCO output multiplier.
 * P: SYSCLK output division factor.
 * Q: SDIO, RNG, and USB output division factor.
 *
 * Return 0 upon success 1 otherwise.
 */
uint8_t RCC_set_PLL(sysclk_src_t src, uint32_t M, uint32_t N, uint32_t P, uint32_t Q){
	if(src != HSI || src != HSE)
		return 1;
	if(Q < 2 || Q > 15)
		return 1;
	if(N < 2 || N > 432)
		return 1;
	if(M < 2 || M > 63)
		return 1;
	if(P < 2 || P > 8 || P%2 != 0)
		return 1;


	// Turn off the PLL for configuration
	uint32_t state = ((RCC_CR>>24)&1)?1:0; 
	RCC_CR &= ~(1 << 24);

	switch(src){
		case(HSE):
			// Calculate P
			P = (P/2)-1;	
			// Set PLL source to HSE
			RCC_PLLCFGR |= (1 << 22);
			RCC_write_PLL_params(M,N,P,Q);
			break;
		case(HSI):
			uint32_t intermediate1;
			uint32_t intermediate2;
		// HSI uses an internal 16 MHz oscillator
			if(HSI_FRQ/M < MIN_M || (intermediate1=HSI_FRQ/M) > MAX_M){
				if(state)
					RCC_CR |= (1 << 24);
				return 1;
			}
			if(intermediate1 != 0 && N > UINT32_MAX / intermediate1){
				if(state)
					RCC_CR |= (1 << 24);
				return 1; // overflow
			}
			if(intermediate1*N > MAX_N || (intermediate2=intermediate1*N) < MIN_N){
				if(state)
					RCC_CR |= (1 << 24);
				return 1; // overflow
			}
			if(intermediate2/P > 84){
				if(state)
					RCC_CR |= (1 << 24);
				return 1; // overflow
			}
			// Calculate P
			P = (P/2)-1;	
			// Set PLL source to HSI
			RCC_PLLCFGR &= ~(1 << 22);
			RCC_write_PLL_params(M,N,P,Q);
			break;
		default:
			if(state)
				RCC_CR |= (1 << 24);
			return 1; // overflow
	}
	// If PLL was on, turn it back on
	if(state)
		RCC_CR |= (1 << 24);
	
	return 0;
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
 *
 * return 0 upon success and 1 otherwise
 */
uint8_t RCC_clockout(clk_t clk, MCO_t mco, uint32_t prescaler){
	if(prescaler < 1 || prescaler > 5)
		return 1;	

	if(mco == MCO1){
		switch(clk){
			case(clk_LSE):
				RCC_CFGR &= ~(0x3 << 21);
				RCC_CFGR |= (0x1 << 21);
				break;
			case(clk_HSI):
				RCC_CFGR &= ~(0x3 << 21);
				break;
			case(clk_HSE):
				RCC_CFGR &= ~(0x3 << 21);
				RCC_CFGR |= (0x2 << 21);
				break;
			case(clk_PLL):
				RCC_CFGR &= ~(0x3 << 21);
				RCC_CFGR |= (0x3 << 21);
				break;
			default:
				return 1;
		}

		// set the prescaler
		prescaler = (prescaler==1)?0:(0x2+prescaler);
		RCC_CFGR &= ~(0x7 << 24);
		if(prescaler)
			RCC_CFGR |= ((0x7 & prescaler) << 24);
	}
	else if(mco == MCO2){
		switch(clk){
			case(clk_HSE):
				RCC_CFGR &= ~(0x3 << 30);
				RCC_CFGR |= (0x2 << 30);
				break;
			case(clk_PLL):
				RCC_CFGR &= ~(0x3 << 30);
				RCC_CFGR |= (0x3 << 30);
				break;
			case(clk_PLLI2S):
				RCC_CFGR &= ~(0x3 << 30);
				RCC_CFGR |= (0x1 << 30);
				break;
			case(clk_SYSCLK):
				RCC_CFGR &= ~(0x3 << 30);
				break;
			default:
				return 1;
		}

		// set the prescaler
		prescaler = (prescaler==1)?0:(0x2+prescaler);
		RCC_CFGR &= ~(0x7 << 27);
		if (prescaler)
			RCC_CFGR |= ((0x7 & prescaler) << 27);
	}
	else
		return 1;

	return 0;
}

/*
 * Reset a peripheral (check `enum peripheral_t`)
 * works by setting and clearing the reset bit
 *
 * returns 0 upon success and 1 otherwise
 */
uint8_t RCC_reset_peripheral(peripheral_t peripheral){
	switch(peripheral){
		case(GPIOA):
			RCC_AHB1RSTR |= (1<<0);
			RCC_AHB1RSTR &= ~(1<<0);
			break;
		case(GPIOB):
			RCC_AHB1RSTR |= (1<<1);
			RCC_AHB1RSTR &= ~(1<<1);
			break;
		case(GPIOC):
			RCC_AHB1RSTR |= (1<<2);
			RCC_AHB1RSTR &= ~(1<<2);
			break;
		case(GPIOD):
			RCC_AHB1RSTR |= (1<<3);
			RCC_AHB1RSTR &= ~(1<<3);
			break;
		case(GPIOE):
			RCC_AHB1RSTR |= (1<<4);
			RCC_AHB1RSTR &= ~(1<<4);
			break;
		case(GPIOH):
			RCC_AHB1RSTR |= (1<<7);
			RCC_AHB1RSTR &= ~(1<<7);
			break;
		case(CRC):
			RCC_AHB1RSTR |= (1<<12);
			RCC_AHB1RSTR &= ~(1<<12);
			break;
		case(DMA1):
			RCC_AHB1RSTR |= (1<<21);
			RCC_AHB1RSTR &= ~(1<<21);
			break;
		case(DMA2):
			RCC_AHB1RSTR |= (1<<22);
			RCC_AHB1RSTR &= ~(1<<22);
			break;
		case(OTGFS):
			RCC_AHB2RSTR |= (1<<7);
			RCC_AHB2RSTR &= ~(1<<7);
			break;
		case(TIM2):
			RCC_APB1RSTR |= (1<<0);
			RCC_APB1RSTR &= ~(1<<0);
			break;
		case(TIM3):
			RCC_APB1RSTR |= (1<<1);
			RCC_APB1RSTR &= ~(1<<1);
			break;
		case(TIM4):
			RCC_APB1RSTR |= (1<<2);
			RCC_APB1RSTR &= ~(1<<2);
			break;
		case(TIM5):
			RCC_APB1RSTR |= (1<<3);
			RCC_APB1RSTR &= ~(1<<3);
			break;
		case(WWDG):
			RCC_APB1RSTR |= (1<<11);
			RCC_APB1RSTR &= ~(1<<11);
			break;
		case(SPI2):
			RCC_APB1RSTR |= (1<<14);
			RCC_APB1RSTR &= ~(1<<14);
			break;
		case(SPI3):
			RCC_APB1RSTR |= (1<<15);
			RCC_APB1RSTR &= ~(1<<15);
			break;
		case(USART2):
			RCC_APB1RSTR |= (1<<17);
			RCC_APB1RSTR &= ~(1<<17);
			break;
		case(I2C1):
			RCC_APB1RSTR |= (1<<21);
			RCC_APB1RSTR &= ~(1<<21);
			break;
		case(I2C2):
			RCC_APB1RSTR |= (1<<22);
			RCC_APB1RSTR &= ~(1<<22);
			break;
		case(I2C3):
			RCC_APB1RSTR |= (1<<23);
			RCC_APB1RSTR &= ~(1<<23);
			break;
		case(PWR):
			RCC_APB1RSTR |= (1<<28);
			RCC_APB1RSTR &= ~(1<<28);
			break;
		case(TIM1):
			RCC_APB2RSTR |= (1<<0);
			RCC_APB2RSTR &= ~(1<<0);
			break;
		case(USART1):
			RCC_APB2RSTR |= (1<<4);
			RCC_APB2RSTR &= ~(1<<4);
			break;
		case(USART6):
			RCC_APB2RSTR |= (1<<5);
			RCC_APB2RSTR &= ~(1<<5);
			break;
		case(ADC1):
			RCC_APB2RSTR |= (1<<8);
			RCC_APB2RSTR &= ~(1<<8);
			break;
		case(SDIO):
			RCC_APB2RSTR |= (1<<11);
			RCC_APB2RSTR &= ~(1<<11);
			break;
		case(SPI1):
			RCC_APB2RSTR |= (1<<12);
			RCC_APB2RSTR &= ~(1<<12);
			break;
		case(SPI4):
			RCC_APB2RSTR |= (1<<13);
			RCC_APB2RSTR &= ~(1<<13);
			break;
		case(SYSCFG):
			RCC_APB2RSTR |= (1<<14);
			RCC_APB2RSTR &= ~(1<<14);
			break;
		case(TIM9):
			RCC_APB2RSTR |= (1<<16);
			RCC_APB2RSTR &= ~(1<<16);
			break;
		case(TIM10):
			RCC_APB2RSTR |= (1<<17);
			RCC_APB2RSTR &= ~(1<<17);
			break;
		case(TIM11):
			RCC_APB2RSTR |= (1<<18);
			RCC_APB2RSTR &= ~(1<<18);
			break;
		default:
			return 1;
	}
	return 0;
}

/*
 * Enable a peripheral (check `enum peripheral_t`)
 *
 * return 0 upon success and 1 otherwise
 *
 */
uint8_t RCC_enable_peripheral(peripheral_t peripheral){

	return RCC_con_peripheral(peripheral, 1);

}

/*
 * Disable a peripheral (check `enum peripheral_t`)
 *
 * return 0 upon success and 1 otherwise
 *
 */
uint8_t RCC_disable_peripheral(peripheral_t peripheral){

	return RCC_con_peripheral(peripheral, 0);

}

/* Helper
 * Set the multiplier/divisor values for the main PLL.
 * Does no checking and returns nothing.
 */
static void RCC_write_PLL_params(uint32_t M, uint32_t N, uint32_t P, uint32_t Q){
	// Clear and then write M, N, P, Q respectively
	RCC_PLLCFGR &= ~((0x3F << 0) | (0x1FF << 6) | (0x03 << 16) | (0x0F << 24));
	RCC_PLLCFGR |= ((0x3F & M) | ((0x1FF & N) << 6) | ((0x03 & P)<< 16) | ((0x0F & Q) << 24));
}

/* Helper
 * RCC control peripheral
 * Backend for the RCC_enable/disable functions
 * saves LOC
 *
 * Returns 0 upon success and 1 otherwise
 *
 */
static uint8_t RCC_con_peripheral(peripheral_t peripheral, uint32_t state){
	if(state != 1 && state != 0)
		return 1;
	switch(peripheral){
		case(GPIOA):
			RCC_AHB1ENR = (RCC_AHB1ENR & ~(1<<0)) | (state << 0);
			break;
		case(GPIOB):
			RCC_AHB1ENR = (RCC_AHB1ENR & ~(1<<1)) | (state << 1);
			break;
		case(GPIOC):
			RCC_AHB1ENR = (RCC_AHB1ENR & ~(1<<2)) | (state << 2);
			break;
		case(GPIOD):
			RCC_AHB1ENR = (RCC_AHB1ENR & ~(1<<3)) | (state << 3);
			break;
		case(GPIOE):
			RCC_AHB1ENR = (RCC_AHB1ENR & ~(1<<4)) | (state << 4);
			break;
		case(GPIOH):
			RCC_AHB1ENR = (RCC_AHB1ENR & ~(1<<7)) | (state << 7);
			break;
		case(CRC):
			RCC_AHB1ENR = (RCC_AHB1ENR & ~(1<<12)) | (state << 12);
			break;
		case(DMA1):
			RCC_AHB1ENR = (RCC_AHB1ENR & ~(1<<21)) | (state << 21);
			break;
		case(DMA2):
			RCC_AHB1ENR = (RCC_AHB1ENR & ~(1<<22)) | (state << 22);
			break;
		case(OTGFS):
			RCC_AHB2ENR = (RCC_AHB2ENR & ~(1<<7)) | (state << 7);
			break;
		case(TIM2):
			RCC_APB1ENR = (RCC_APB1ENR & ~(1<<0)) | (state << 0);
			break;
		case(TIM3):
			RCC_APB1ENR = (RCC_APB1ENR & ~(1<<1)) | (state << 1);
			break;
		case(TIM4):
			RCC_APB1ENR = (RCC_APB1ENR & ~(1<<2)) | (state << 2);
			break;
		case(TIM5):
			RCC_APB1ENR = (RCC_APB1ENR & ~(1<<3)) | (state << 3);
			break;
		case(WWDG):
			RCC_APB1ENR = (RCC_APB1ENR & ~(1<<11)) | (state << 11);
			break;
		case(SPI2):
			RCC_APB1ENR = (RCC_APB1ENR & ~(1<<14)) | (state << 14);
			break;
		case(SPI3):
			RCC_APB1ENR = (RCC_APB1ENR & ~(1<<15)) | (state << 15);
			break;
		case(USART2):
			RCC_APB1ENR = (RCC_APB1ENR & ~(1<<17)) | (state << 17);
			break;
		case(I2C1):
			RCC_APB1ENR = (RCC_APB1ENR & ~(1<<21)) | (state << 21);
			break;
		case(I2C2):
			RCC_APB1ENR = (RCC_APB1ENR & ~(1<<22)) | (state << 22);
			break;
		case(I2C3):
			RCC_APB1ENR = (RCC_APB1ENR & ~(1<<23)) | (state << 23);
			break;
		case(PWR):
			RCC_APB1ENR = (RCC_APB1ENR & ~(1<<28)) | (state << 28);
			break;
		case(TIM1):
			RCC_APB2ENR = (RCC_APB2ENR & ~(1<<0)) | (state << 0);
			break;
		case(USART1):
			RCC_APB2ENR = (RCC_APB2ENR & ~(1<<4)) | (state << 4);
			break;
		case(USART6):
			RCC_APB2ENR = (RCC_APB2ENR & ~(1<<5)) | (state << 5);
			break;
		case(ADC1):
			RCC_APB2ENR = (RCC_APB2ENR & ~(1<<8)) | (state << 8);
			break;
		case(SDIO):
			RCC_APB2ENR = (RCC_APB2ENR & ~(1<<11)) | (state << 11);
			break;
		case(SPI1):
			RCC_APB2ENR = (RCC_APB2ENR & ~(1<<12)) | (state << 12);
			break;
		case(SPI4):
			RCC_APB2ENR = (RCC_APB2ENR & ~(1<<13)) | (state << 13);
			break;
		case(SYSCFG):
			RCC_APB2ENR = (RCC_APB2ENR & ~(1<<14)) | (state << 14);
			break;
		case(TIM9):
			RCC_APB2ENR = (RCC_APB2ENR & ~(1<<16)) | (state << 16);
			break;
		case(TIM10):
			RCC_APB2ENR = (RCC_APB2ENR & ~(1<<17)) | (state << 17);
			break;
		case(TIM11):
			RCC_APB2ENR = (RCC_APB2ENR & ~(1<<18)) | (state << 18);
			break;
		default:
			return 1;
	}
	return 0;
}
