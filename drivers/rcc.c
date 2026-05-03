/* 
 * RCC (Reset and clock control) module source
 * Date: Sun Apr 19 02:36:35 PM EET 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#include <stdint.h>
#include <limits.h>
#include <../Inc/drivers/rcc.h>

static void RCC_write_PLL_params(uint32_t M, uint32_t N, uint32_t P, uint32_t Q);
static uint8_t RCC_con_peripheral(peripheral_t peripheral, uint32_t state, uint32_t lp);
static uint8_t RCC_get_PLL_params(uint32_t* M, uint32_t* N, uint32_t* P, uint32_t* Q);
static uint8_t RCC_check_PLL_freq_flow(sysclk_src_t src, uint32_t M, uint32_t N, uint32_t P, uint32_t Q);

/*
 *
 * A peripheral lookup table
 * See (`typedef struct peripheral_reg_t`)
 * For each bus, there is a register for enabling/disabling and
 * another for reseting certain peripherals. A bus has the same offset
 * for all peripherals across all registers.
 *
 * For example, GPIOC is on the AHB1 bus. Its reset bit is the 2nd bit
 * in the AHB1RSTR and its enable/disable bit is also the 2nd bit in the
 * AHB1ENR.
 *
 * The address of the low power enable register and the reset register can
 * be obtained from the enable register via a +0x20 and a -0x20 offset respectively.
 */
static const peripheral_reg_t peripheral_lookup_table[] = {
    [GPIOA]  = {ADDR_RCC_AHB1ENR, 0},
    [GPIOB]  = {ADDR_RCC_AHB1ENR, 1},
    [GPIOC]  = {ADDR_RCC_AHB1ENR, 2},
    [GPIOD]  = {ADDR_RCC_AHB1ENR, 3},
    [GPIOE]  = {ADDR_RCC_AHB1ENR, 4},
    [GPIOH]  = {ADDR_RCC_AHB1ENR, 7},
    [CRC]    = {ADDR_RCC_AHB1ENR, 12},
    [DMA1]   = {ADDR_RCC_AHB1ENR, 21},
    [DMA2]   = {ADDR_RCC_AHB1ENR, 22},
    [OTGFS]  = {ADDR_RCC_AHB2ENR, 7},
    [TIM2]   = {ADDR_RCC_APB1ENR, 0},
    [TIM3]   = {ADDR_RCC_APB1ENR, 1},
    [TIM4]   = {ADDR_RCC_APB1ENR, 2},
    [TIM5]   = {ADDR_RCC_APB1ENR, 3},
    [WWDG]   = {ADDR_RCC_APB1ENR, 11},
    [SPI2]   = {ADDR_RCC_APB1ENR, 14},
    [SPI3]   = {ADDR_RCC_APB1ENR, 15},
    [USART2] = {ADDR_RCC_APB1ENR, 17},
    [I2C1]   = {ADDR_RCC_APB1ENR, 21},
    [I2C2]   = {ADDR_RCC_APB1ENR, 22},
    [I2C3]   = {ADDR_RCC_APB1ENR, 23},
    [PWR]    = {ADDR_RCC_APB1ENR, 28},
    [TIM1]   = {ADDR_RCC_APB2ENR, 0},
    [USART1] = {ADDR_RCC_APB2ENR, 4},
    [USART6] = {ADDR_RCC_APB2ENR, 5},
    [ADC1]   = {ADDR_RCC_APB2ENR, 8},
    [SDIO]   = {ADDR_RCC_APB2ENR, 11},
    [SPI1]   = {ADDR_RCC_APB2ENR, 12},
    [SPI4]   = {ADDR_RCC_APB2ENR, 13},
    [SYSCFG] = {ADDR_RCC_APB2ENR, 14},
    [TIM9]   = {ADDR_RCC_APB2ENR, 16},
    [TIM10]  = {ADDR_RCC_APB2ENR, 17},
    [TIM11]  = {ADDR_RCC_APB2ENR, 18},
};

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
				// TODO: better add a countdown here
			}

			// select the clock
			RCC_CFGR &= ~(0x3<<0);
			RCC_CFGR |= (0x1<<0);
			while(((RCC_CFGR >> 2) & (0x03)) != 0x01){
				// TODO: better add a countdown here
			}
			break;
		case(HSI):
			RCC_CR |= (1 << 0);
			while(!(RCC_CR & (1 << 1))){
				// TODO: better add a countdown here
			}

			// select the clock
			RCC_CFGR &= ~(0x3<<0);
			RCC_CFGR |= (0x0<<0);
			while(((RCC_CFGR >> 2) & (0x03)) != 0x00){
				// TODO: better add a countdown here
			}
			break;
		// PLL should be configured (check `RCC_set_PLL`)
		case(PLL):
			RCC_CR |= (1 << 24);
			while(!(RCC_CR & (1 << 25))){
				// TODO: better add a countdown here
			}

			// select the clock
			RCC_CFGR &= ~(0x3<<0);
			RCC_CFGR |= (0x1<<1);
			while(((RCC_CFGR >> 2) & (0x03)) != 0x1<<1){
				// TODO: better add a countdown here
			}
			break;
		default:
			return 1;
	}
	return 0; 
}

/*
 * Set the prescaler for a specific bus.
 *
 * NOTE: For the APBx buses, the input is the AHB bus clock.
 *		 For the AHB bus, the input is the system clock.
 *
 * Return 0 upon success and 1 otherwise.
 */
uint8_t RCC_set_bus_prescaler(bus_t bus, uint32_t prescaler){
	switch(prescaler){
		case(1):
			prescaler = 0;
			break;
		case(2):
			prescaler = 8;
			break;
		case(4):
			prescaler = 9;
			break;
		case(8):
			prescaler = 10;
			break;
		case(16):
			prescaler = 11;
			break;
		case(64):
			prescaler = 12;
			break;
		case(128):
			prescaler = 13;
			break;
		case(256):
			prescaler = 14;
			break;
		case(512):
			prescaler = 15;
			break;
		default:
			return 1;
	}

	switch(bus){
		case(AHB):
			RCC_CFGR &= ~(0x0F << 4);	
			RCC_CFGR |= ((prescaler & 0x0F) << 4);	
			break;
		case(APB1):
			RCC_CFGR &= ~(0x07 << 10);	
			RCC_CFGR |= ((prescaler & 0x07) << 10);	
			break;
		case(APB2):
			RCC_CFGR &= ~(0x07 << 13);	
			RCC_CFGR |= ((prescaler & 0x07) << 13);	
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
	if(peripheral >= sizeof(peripheral_lookup_table)/sizeof(peripheral_lookup_table[0]))
		return 1;
	volatile uint32_t* reg = peripheral_lookup_table[peripheral].enr;
	uint32_t bit = (peripheral_lookup_table[peripheral].bit_position);

	// Reset register of any bus is at an offset of -0x20 from the Enable register
	// 0x08 is 0x20/0x04 for pointer arithmetic
	reg-= 0x08;

	*reg |= 1<<bit;
	*reg &= ~(1<<bit);

	return 0;
}

/*
 * Enable a peripheral (check `enum peripheral_t`)
 *
 * return 0 upon success and 1 otherwise
 *
 */
uint8_t RCC_enable_peripheral(peripheral_t peripheral){

	return RCC_con_peripheral(peripheral, 1, 0);

}

/*
 * Disable a peripheral (check `enum peripheral_t`)
 *
 * return 0 upon success and 1 otherwise
 *
 */
uint8_t RCC_disable_peripheral(peripheral_t peripheral){

	return RCC_con_peripheral(peripheral, 0, 0);

}

/*
 * Enable a peripheral when the deivce is sleep mode (check `enum peripheral_t`)
 *
 * return 0 upon success and 1 otherwise
 *
 */
uint8_t RCC_enable_LP_peripheral(peripheral_t peripheral){

	return RCC_con_peripheral(peripheral, 1, 1);

}

/*
 * Disable a peripheral when the deivce is sleep mode (check `enum peripheral_t`)
 *
 * return 0 upon success and 1 otherwise
 *
 */
uint8_t RCC_disable_LP_peripheral(peripheral_t peripheral){

	return RCC_con_peripheral(peripheral, 0, 1);

}

/*
 * Initializes the PLLI2S
 * NOTE: The main PLL controls the input clock frequency source and the M multiplier
 *
 * Return 0 upon success, 1 otherwise
 */
uint8_t RCC_set_PLLI2S(uint32_t R, uint32_t N){
	clk_t pll_clk;
	uint32_t freq;
	uint32_t M;

	if(R > MAX_R || R < MIN_R)
		return 1;
	if(N > MAX_N || N < MIN_N)
		return 1;
		
	uint32_t state = ((RCC_CR >> 26) & 0x01)?1:0;
	RCC_CR &= ~(1 << 26);

	// Determine which clock source the PLL is using
	// (this is set by the main PLL)	
	pll_clk = ((RCC_PLLCFGR >> 22) & 0x01)?clk_HSE:clk_HSI;
	// frequency of HSI is 16 MHz
	freq = (pll_clk==clk_HSI)?HSI_FRQ:HSE_FRQ;
	// Read the M value (set by the main PLL)
	M = (RCC_PLLCFGR & 0x3F);
	
	if(M < MIN_M || M > MAX_M){
		if(state)
			RCC_CR |= (1 << 26);
		return 1;
	}
	uint32_t vco_in = freq / M;
	if(N > UINT32_MAX / vco_in){ /* overflow */
		if(state)
			RCC_CR |= (1 << 26);
		return 1;
	}
	uint32_t vco = vco_in * N;
	if(vco > MAX_N_FRQ || vco < MIN_N_FRQ){
		if(state)
			RCC_CR |= (1 << 26);
		return 1;
	}

	// Write the R division factor
	RCC_PLLI2SCFGR &= ~(0x7 << 28);
	RCC_PLLI2SCFGR |= (R << 28);

	// Write the N multiplier
	RCC_PLLI2SCFGR &= ~(0x1FF << 6);
	RCC_PLLI2SCFGR |= (N << 6);

	if(state)
		RCC_CR |= (1 << 26);
	return 0;	
}

/*
 * Initialize the RTC clock
 * NOTE: RTCCLK shouldn't exceed 100 MHz
 *
 * clk: input clock source (either LSE, LSI, or HSE)
 * prescaler: divisor in case HSE is used as clock input
 *
 * Return 0 upon success, 1 otherwise
 *
 */
uint8_t RCC_set_RTC(clk_t clk, uint32_t prescaler){
	uint32_t freq;
	switch(clk){
		case(clk_LSE):
			// Set the RCC source
			RCC_BDCR &= ~(0x3 << 8);
			RCC_BDCR |= (0x1 << 8);
			break;
		case(clk_LSI):
			// Set the RCC source
			RCC_BDCR &= ~(0x3 << 8);
			RCC_BDCR |= (0x2 << 8);
			break;
		case(clk_HSE):
			freq = HSE_FRQ;
			if(prescaler < 2 || prescaler > 0x1FF)
				return 1;
			freq /= prescaler;
			if(freq > MAX_RTC_FRQ)
				return 1;
			RCC_CFGR &= ~(0x1F << 16);
			RCC_CFGR |= ((prescaler & 0x1F) << 16);

			// Set the RCC source
			RCC_BDCR &= ~(0x3 << 8);
			RCC_BDCR |= (0x3 << 8);
			break;
		default:
			return 1;
	}
	return 0;
}

/*
 * Initialize the SSM (spread spectrum modulation) clock
 * NOTE: This function should be called before the main PLL is enabled or after it's disabled
 *
 * TODO: parameter description
 *
 * Return 0 upon success, 1 otherwise
 *
 */
uint8_t RCC_set_SSM(uint32_t modulation_period, uint32_t inc_step, uint32_t spread_select){
	uint32_t state = ((RCC_CR >> 24) & 0x01)?1:0;
	if(RCC_disable_PLL())
		return 1;
	if(spread_select > 1 || inc_step > 0x7FFF || modulation_period > 0x1FFF){
		if(state){
			if(RCC_enable_PLL())
				return 1;
		}
		return 1;
	}

	RCC_SSCGR &= ~(1<<30);
	RCC_SSCGR |= ((spread_select & 0x1) << 30);

	RCC_SSCGR &= ~(0x7FFF << 13);
	RCC_SSCGR |= ((inc_step & 0x7FFF) << 13);

	RCC_SSCGR &= ~(0x1FFF);
	RCC_SSCGR |= (modulation_period & 0x1FFF);

	if(state){
		 if(RCC_enable_PLL())
			return 1;
	}
	return 0;
}

/*
 * Set the clock prescaler for the timers using the APBx buses
 * NOTE: If the bus prescaler (configured using `RCC_bus_prescaler()`) for the APBx buses is set to 1,
 * 		 the clock used for these timers is just the HCLK (AHB bus clock)
 *
 * prescaler: either 4 or 2.
 *
 * NOTE: In case the APBx bus prescaler is 1, the timer clock is HCLK regardless of this setting.
 *
 * Return 0 upon success, 1 otherwise
 *
 */
uint8_t RCC_set_TIM_prescaler(uint32_t prescaler){
	if(prescaler == 2)
		RCC_DCKCFGR &= ~(1 << 24);
	else if(prescaler == 4)
		RCC_DCKCFGR |= (1 << 24);
	else
		return 1;
	return 0;	
}

/*
 * Enable the RTC clock
 */
static inline void RCC_enable_RTC(void){
	RCC_BDCR |= 1<<16;
}

/*
 * Disable the RTC clock
 */
static inline void RCC_disable_RTC(void){
	RCC_BDCR &= ~(1<<16);
}

/*
 * Enable the main PLL and wait till it's locked
 *
 * Return 0 upon success, 1 otherwise
 *
 */
uint8_t RCC_enable_PLL(void){
	RCC_CR |= (1 << 24);
	while(!((RCC_CR >> 25) & 0x1)){
		// TODO: better add a countdown here
	}

	return 0;
}

/*
 * Disable the main PLL and wait till it's unlocked
 * NOTE: You can't disable the PLL if it's used as system clock
 *
 * Return 0 upon sucess, 1 otherwise
 *
 */
uint8_t RCC_disable_PLL(void){
	// Can't disable if PLL is system clock
	// If the PLL is off, there is no reason to return false
	uint32_t isiton = (RCC_CR >> 24) & 0x01;
	if((((RCC_CFGR >> 2) & 0x3) == 0x2) && isiton)
		return 1;
	// No reason to clear a cleared bit!
	if(isiton){
		RCC_CR &= ~(1 << 24);
		while((RCC_CR >> 25) & 0x1){
			// TODO: better add a countdown here
		}
	}
	return 0;
}

/*
 * Enable the PLLI2S and wait till it's locked
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t RCC_enable_PLLI2S(void){
	RCC_CR |= (1 << 26);
	while(!((RCC_CR >> 27) & 0x1)){
		// TODO: better add a countdown here
	}

	return 0;
}

/*
 * Disable the PLLI2S and wait till it's unlocked
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t RCC_disable_PLLI2S(void){
	RCC_CR &= ~(1 << 26);
	while((RCC_CR >> 27) & 0x1){
		// TODO: better add a countdown here
	}

	return 0;
}

/*
 * Get the current system clock frequency.
 *
 * freq: A pointer to a uint32_t to store the frequency in.
 *
 * Return 0 upon success and 1 otherwise.
 */
uint32_t RCC_get_SYSCLK_freq(uint32_t* freq){
	uint32_t M,N,P,Q;
	uint32_t pfreq, qfreq;
	if(freq == NULL)
		return 1;
	// read the current sysclock clock source
	uint32_t temp = (RCC_CFGR >> 2) & 0x3;
	switch(temp){
		case(0): // HSI
			*freq = HSI_FRQ;
			return 0;
		case(1): // HSE
			*freq = HSE_FRQ;
			return 0;
		case(2): // PLL
			if(RCC_get_PLL_params(&M,&N,&P,&Q))
				return 1;
			if(RCC_get_PLL_clkout(&pfreq, &qfreq))
				return 1;
			*freq = pfreq;
			break;
		default:
			return 1;
	}
	return 0;
}

/*
 * Get the output frequencies of the main PLL
 * NOTE: A certain clock source can be used as input to the PLL but this doesn't
 * 		 mean that it's stable. This is why there is a check on the XXXRDY bits.
 *
 * pfreq: The output frequency after the P divisor (goes to systemclk MUX)
 * qfreq: The output frequency after the Q divisor (goes to the USB_OTG, SDIO, and RNG)
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint32_t RCC_get_PLL_clkout(uint32_t* pfreq, uint32_t* qfreq){
	sysclk_src_t src;
	if(pfreq == NULL || qfreq == NULL)
		return 1;
	uint32_t M, N, P, Q;
	uint32_t freq;
	if(RCC_get_PLL_params(&M, &N, &P, &Q))
		return 1;
	// Read the PLL clock source and check the ready flag
	if(((RCC_PLLCFGR >> 22) & 0x01) && ((RCC_CR >> 17) & 0x01)){
		src = HSE;	
		freq = HSE_FRQ;
	}
	else if(!((RCC_PLLCFGR >> 22) & 0x01) && ((RCC_CR >> 1) & 0x01)){
		src = HSI;	
		freq = HSI_FRQ;
	}
	else
		return 1;
	if(RCC_check_PLL_freq_flow(src, M, N, P, Q))
		return 1;
	uint32_t vco = (freq/M)*N;
	*pfreq = vco/P;
	*qfreq = vco/Q;
	return 0;
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
 * Backend for all RCC_en/dis functions
 *
 * state: 0 for disable, 1 for enable
 * lp: 0 for normal mode, 1 for low power mode
 *
 * Returns 0 upon success and 1 otherwise
 *
 */
static uint8_t RCC_con_peripheral(peripheral_t peripheral, uint32_t state, uint32_t lp){
	if(peripheral >= sizeof(peripheral_lookup_table)/sizeof(peripheral_lookup_table[0]))
		return 1;
	if(state != 1 && state != 0)
		return 1;
	// In case of low power operation, the low power enable register is exactly at a +0x20 offset (0x20/0x04 in pointer arith)
	volatile uint32_t* reg = (lp)?(peripheral_lookup_table[peripheral].enr+0x08):(peripheral_lookup_table[peripheral].enr);
	uint32_t bit = (peripheral_lookup_table[peripheral].bit_position);

	*reg = (*reg & ~(1<<bit)) | (state < bit);

	return 0;
}

/* Helper
 *
 * Read the current PLL division/prescaler values
 * NOTE: the factors are the divisor values not the register values
 * 		 It's the case for most factors to match their register values
 * 		 only P is as exception.
 *
 * M,N,P, and Q: pointers to uint32_t to store the factors
 *
 * Return 0 upon success and 1 otherwise
 *
 */
static uint8_t RCC_get_PLL_params(uint32_t* M, uint32_t* N, uint32_t* P, uint32_t* Q){
	if(M == NULL || N == NULL || P == NULL || Q == NULL)
		return 1;
	*M = RCC_PLLCFGR & 0x3F;
	*N = (RCC_PLLCFGR >> 6) & 0x1FF;
	*P = (RCC_PLLCFGR >> 16) & 0x03;
	*P = ((*P)+1)*2;
	*Q = (RCC_PLLCFGR >> 24) & 0x0F;
	return 0;
}

/* Helper
 *
 * Check the frequency flow for the main PLL.
 * Any frequency violation within the PLL will return 1.
 *
 * Returns 0 upon success, 1 otherwise
 *
 */
static uint8_t RCC_check_PLL_freq_flow(sysclk_src_t src, uint32_t M, uint32_t N, uint32_t P, uint32_t Q){
	uint32_t intermediate1;
	uint32_t intermediate2;
	uint32_t intermediate3;
	uint32_t src_freq;
	if(M == 0 || Q == 0)
		return 1;	
	if(src == HSI)
		src_freq = HSI_FRQ;
	else if(src == HSE)
		src_freq = HSE_FRQ;
	else
		return 1;

	intermediate1 = src_freq / M;
	// HSI uses an internal 16 MHz oscillator
	if(src_freq/M < MIN_M_FRQ || intermediate1 > MAX_M_FRQ)
		return 1;
	if(intermediate1 != 0 && N > UINT32_MAX / intermediate1)
		return 1; // overflow
	intermediate2 = intermediate1 * N;
	if(intermediate1*N > MAX_N_FRQ || intermediate2 < MIN_N_FRQ)
		return 1; // overflow
	if(intermediate2/P > MAX_P_FRQ)
		return 1; // overflow
	intermediate3 = intermediate2/Q;	
	if(intermediate3 > MAX_Q_FRQ)
		return 1;
	return 0;
}

