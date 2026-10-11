/*
 * GPIO module header
 * Date: Tue Dec 4 2025
 * Updated on: Sun Oct 4 01:24:31 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>
#include <drivers/rcc.h>

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

typedef struct {
	uint32_t moder, ospeedr, pupdr;
} gpio_reset_t;

typedef enum {
	GPIO_PORTA, GPIO_PORTB, GPIO_PORTC, GPIO_PORTD, GPIO_PORTE, GPIO_PORTH =
	    7
} gpio_port;

typedef enum {
	GPIO_PIN0, GPIO_PIN1, GPIO_PIN2, GPIO_PIN3, GPIO_PIN4, GPIO_PIN5,
	GPIO_PIN6, GPIO_PIN7, GPIO_PIN8, GPIO_PIN9, GPIO_PIN10,
	GPIO_PIN11, GPIO_PIN12, GPIO_PIN13, GPIO_PIN14, GPIO_PIN15
} gpio_pin;

/* Output mode */
typedef enum {
	GPIO_PP,		/* Push pull */
	GPIO_OD			/* Open drain */
} gpio_output_mode;

/* Speed */
typedef enum {
	GPIO_low,
	GPIO_med,
	GPIO_fast,
	GPIO_high
} gpio_speed;

/* Mode */
typedef enum {
	GPIO_in,
	GPIO_out,
	GPIO_analog = 3,
	GPIO_af0, GPIO_af1, GPIO_af2, GPIO_af3,
	GPIO_af4, GPIO_af5, GPIO_af6, GPIO_af7,
	GPIO_af8, GPIO_af9, GPIO_af10, GPIO_af11,
	GPIO_af12, GPIO_af13, GPIO_af14, GPIO_af15
} gpio_mode;

/* Internal resistor */
typedef enum {
	GPIO_pull_up,		/* Push up */
	GPIO_pull_down		/* Pull down */
} gpio_resistor_mode;

typedef struct {
	// GPIO mode
	gpio_mode mode;
	// push pull or open drain
	gpio_output_mode out_type;
	// speed
	gpio_speed out_speed;
	//internal push/pull resistor used
	gpio_resistor_mode resistor_mode;
} gpio_config_t;

#define GPIOA_BASE     0x40020000
#define GPIOB_BASE     0x40020400
#define GPIOC_BASE     0x40020800

#define ADDR_GPIO_A ((volatile uint32_t*)(GPIOA_BASE))
#define ADDR_GPIO_B ((volatile uint32_t*)(GPIOA_BASE))
#define ADDR_GPIO_C ((volatile uint32_t*)(GPIOA_BASE))

typedef struct {
	volatile uint32_t MODER;	// 0x00
	volatile uint32_t OTYPER;	// 0x04
	volatile uint32_t OSPEEDR;	// 0x08
	volatile uint32_t PUPDR;	// 0x0C
	volatile uint32_t IDR;	// 0x10
	volatile uint32_t ODR;	// 0x14
	volatile uint32_t BSRR;	// 0x18
	volatile uint32_t LCKR;	// 0x1C
	volatile uint32_t AFRL;	// 0x20
	volatile uint32_t AFRH;	// 0x24
} GPIO_regs;

/*
 * Initialize a GPIO pin
 *
 * gpio: A pointer to a gpio_config_t struct (See gpio.h for gpio_config_t fields)
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t GPIO_init(gpio_port port, gpio_pin pin, gpio_config_t * gpio);

/*
 * Return the GPIO to its reset state
 *
 * NOTE: The `gpio_lookup_table` struct contains reset values for
 * 	 the different registers in each port
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t GPIO_deinit(gpio_port port, gpio_pin pin);

/*
 * Set pin to a HIGH state
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t GPIO_set(gpio_port port, gpio_pin pin);

/*
 * Set pin to a LOW state
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t GPIO_clear(gpio_port port, gpio_pin pin);

/*
 * Toggle pin state
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t GPIO_toggle(gpio_port port, gpio_pin pin);

/*
 * Read the state on a GPIO pin
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t GPIO_read(gpio_port port, gpio_pin pin, uint8_t * saved);

/*
 * Change the mode of a pin
 *
 * gpio_mode: GPIO mode (See gpio.h)
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t GPIO_set_mode(gpio_port port, gpio_pin pin, gpio_mode mode);

/*
 * Lock the GPIO registers for some pin
 *
 * NOTE: The lock doesn't apply to GPIOx_BSRR, GPIOx_ODR, and GPIOx_IDR
 * NOTE: This action cannot be undone. The lock is removed only
 * 	 at reset
 */
uint8_t GPIO_lock(gpio_port port, gpio_pin pin);

/*
 * Return pin lock status. 1 if locked, 0 if unlocked, 255 otherwise
 */
uint8_t GPIO_is_locked(gpio_port port, gpio_pin pin);

#endif				/* GPIO_H_ */
