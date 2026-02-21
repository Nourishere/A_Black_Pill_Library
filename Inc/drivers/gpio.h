/*
 * GPIO.h
 *
 *  Created on: Dec 4, 2025
 *      Author: Nour
 * @Brief: Header for the GPIO peripherals.
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>
#include <return.h>
/*
 * Each general-purpose I/O port has four 32-bit configuration registers (GPIOx_MODER,
 * GPIOx_OTYPER, GPIOx_OSPEEDR and GPIOx_PUPDR), two 32-bit data registers
 * (GPIOx_IDR and GPIOx_ODR), a 32-bit set/reset register (GPIOx_BSRR), a 32-bit locking
 * register (GPIOx_LCKR) and two 32-bit alternate function selection register (GPIOx_AFRH
 * and GPIOx_AFRL).
 *
 * The STM32F411CEU6 only has PORTA, PORTB, and C13-15 exposed.
 *
 * GPIOx_MODER -> configures the mode (input, output, AF, or analog.)
 * GPIOx_OTYPER -> configures the output type (push-pull or open drain.)
 * GPIOx_OSPEEDR -> configures the speed of the GPIO.
 * GPIOx_PUPDR -> configures whether to use the internal PU or PD resistor.
 * GPIOx_ODR -> contains the data to be put on the GPIO.
 * GPIOx_IDR -> contains the input data.
 *
 */

#define PORTA 0
#define PORTB 1
#define PORTC 2

#define PIN0  0
#define PIN1  1
#define PIN2  2
#define PIN3  3
#define PIN4  4
#define PIN5  5
#define PIN6  6
#define PIN7  7
#define PIN8  8
#define PIN9  9
#define PIN10  10
#define PIN11 11
#define PIN12 12
#define PIN13 13
#define PIN14 14
#define PIN15 15

/* Output mode */
#define PP 0 /* Push pull */
#define OD 1 /* Open drain */

/* Speed */
#define LOW 0
#define MED 1
#define FAST 2
#define HIGH 3

/* Mode */
#define IN     0
#define OUT    1 
#define AF     2
#define ANALOG 3

/* Internal resistor */
#define PU 1 /* Push up */
#define PD 2 /* Pull down */

#define GPIOA_BASE     0x40020000
#define GPIOB_BASE     0x40020400
#define GPIOC_BASE     0x40020800

typedef struct {
	volatile uint32_t MODER;   // 0x00
	volatile uint32_t OTYPER;  // 0x04
	volatile uint32_t OSPEEDR; // 0x08
	volatile uint32_t OPUPDR;  // 0x0C
	volatile uint32_t IDR;     // 0x10
	volatile uint32_t ODR;     // 0x14
	volatile uint32_t BSRR;    // 0x18
	volatile uint32_t LCKR;    // 0x1C
	volatile uint32_t AFRL;    // 0x20
	volatile uint32_t AFRH;    // 0x24
}GPIO_Regs;

typedef struct {
	/* port number */
	volatile uint8_t port;
	/* pin number */
	volatile uint8_t pin;
	/* GPIO mode
 	 * use IN, OUT, AF, or ANALOG
 	 */
	volatile uint8_t mode;
	/* push pull or open drain
 	 * use PP or OD 
 	 */
	volatile uint8_t out_type;
	/* speed 
     * use LOW, MED, FAST, and HIGH
     */
	volatile uint8_t out_speed;
	/* internal push/pull resistor used 
     * use 0, PU, PD
     */
	volatile uint8_t pp_res;
}GPIO_t;

#define GPIOA ((GPIO_Regs*) GPIOA_BASE)
#define GPIOB ((GPIO_Regs*) GPIOB_BASE)
#define GPIOC ((GPIO_Regs*) GPIOC_BASE)

/* Initialize a GPIO pin with default settings.
 * (Push-pull, no internal resistor, and high speed.) 
 * Takes a port and pin numbers (use macros specified upove.)
 * Return status uint32_t.
 */
GPIO_t  	GPIO_default(uint8_t pin, uint8_t port);

/* Initialize a GPIO pin
 * Return status uint32_t
 k*/
uint32_t	GPIO_init (GPIO_t* gpio);		

/* Set a GPIO pin
 * Return status uint32_t
 */
uint32_t 	GPIO_set (GPIO_t* gpio);

/* Clear a GPIO pin
 * Return status uint32_t
 */
uint32_t 	GPIO_clear (GPIO_t* gpio);

/* Toggle a GPIO pin
 * Return status uint32_t
 */
uint32_t 	GPIO_toggle (GPIO_t* gpio);

/* Read from a GPIO pin
 * Return status uint32_t
 */
uint32_t	GPIO_read (GPIO_t* gpio, uint32_t* saved);

#endif /* GPIO_H_ */
