/*
 * GPIO Module
 * Date: Tue Dec 4 2025
 * Updated on: Sun Oct 4 01:24:31 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */
#include <drivers/gpio.h>

static uint8_t GPIO_chk_inv(gpio_port port, gpio_pin pin);

/*
 * GPIO lookup table
 */
static const peripheral_reg_t gpio_lookup_table[] = {
	[GPIO_PORTA] = {ADDR_GPIO_A, 0},
	[GPIO_PORTB] = {ADDR_GPIO_B, 1},
	[GPIO_PORTC] = {ADDR_GPIO_C, 2},
};

/*
 * GPIO table holding reset values
 * See gpio.h (`enum gpio_reset_t`)
 */
static const gpio_reset_t gpio_reset_table[] = {
	[GPIO_PORTA] = {0xA8000000u, 0x0C000000u, 0x64000000u},
	[GPIO_PORTB] = {0x00000280u, 0x000000C0u, 0x00000100u},
	[GPIO_PORTC] = {0, 0, 0},
	[GPIO_PORTD] = {0, 0, 0},
	[GPIO_PORTE] = {0, 0, 0},
	[GPIO_PORTH] = {0, 0, 0},
};

/*
 * Initialize a GPIO pin
 *
 * gpio: A pointer to a gpio_config_t struct (See gpio.h for gpio_config_t fields)
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t GPIO_init(gpio_port port, gpio_pin pin, gpio_config_t *gpio)
{
	if (gpio == 0 || gpio->mode > GPIO_af7 || gpio->out_speed > GPIO_high
	    || gpio->resistor_mode > GPIO_pull_down || gpio->out_type > GPIO_OD)
		return 1;

	if (GPIO_chk_inv(port, pin))
		return 1;

	GPIO_regs *reg = (GPIO_regs *) (gpio_lookup_table[port].enr);

	reg->MODER &= ~(0x3 << (pin << 1));
	if (gpio->mode >= GPIO_af0)
		reg->MODER |= 0x2 << (pin << 1);
	else
		reg->MODER |= (gpio->mode) << (pin << 1);

	reg->OTYPER &= ~(0x1 << pin);
	reg->OTYPER |= (gpio->out_type) << pin;
	reg->OSPEEDR &= ~(0x3 << (pin << 1));
	reg->OSPEEDR |= (gpio->out_speed) << (pin << 1);
	reg->PUPDR &= ~(0x3 << (pin << 1));
	reg->PUPDR |= (gpio->resistor_mode) << (pin << 1);

	// Handle Alternate Functions
	if (gpio->mode >= GPIO_af0) {
		if (pin > GPIO_PIN7) {
			reg->AFRH &= ~(0xF << (pin << 2));
			reg->AFRH |=
			    ((uint32_t) (gpio->mode - GPIO_af0) & 0x0F) << (pin
									    <<
									    2);
		} else {
			reg->AFRL &= ~(0xF << (pin << 2));
			reg->AFRL |=
			    ((uint32_t) (gpio->mode - GPIO_af0) & 0x0F) << (pin
									    <<
									    2);
		}
	}
	// Enable the clock
	if (RCC_enable_peripheral((peripheral_t) port))
		return 1;

	return 0;
}

/*
 * Return the GPIO to its reset state
 *
 * NOTE: The `gpio_lookup_table` struct contains reset values for
 * 	 the different registers in each port
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t GPIO_deinit(gpio_port port, gpio_pin pin)
{
	if (GPIO_chk_inv(port, pin))
		return 1;

	GPIO_regs *reg = (GPIO_regs *) (gpio_lookup_table[port].enr);

	reg->OSPEEDR = gpio_reset_table[port].ospeedr;
	reg->PUPDR = gpio_reset_table[port].pupdr;
	reg->MODER = gpio_reset_table[port].moder;

	return 0;
}

/*
 * Set pin to a HIGH state
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t GPIO_set(gpio_port port, gpio_pin pin)
{
	if (GPIO_chk_inv(port, pin))
		return 1;

	GPIO_regs *reg = (GPIO_regs *) (gpio_lookup_table[port].enr);

	// See if the pin is not set as output
	if (((reg->MODER >> (1 << pin)) & 0x3) != 0x1)
		return 1;

	reg->BSRR = 1 << pin;

	return 0;
}

/*
 * Set pin to a LOW state
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t GPIO_clear(gpio_port port, gpio_pin pin)
{
	if (GPIO_chk_inv(port, pin))
		return 1;

	GPIO_regs *reg = (GPIO_regs *) (gpio_lookup_table[port].enr);

	// See if the pin is not set as output
	if (((reg->MODER >> (1 << pin)) & 0x3) != 0x1)
		return 1;

	reg->BSRR = 0x10000 << pin;

	return 0;
}

/*
 * Toggle pin state
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t GPIO_toggle(gpio_port port, gpio_pin pin)
{
	if (GPIO_chk_inv(port, pin))
		return 1;

	GPIO_regs *reg = (GPIO_regs *) (gpio_lookup_table[port].enr);

	// See if the pin is not set as output
	if (((reg->MODER >> (1 << pin)) & 0x3) != 0x1)
		return 1;

	uint8_t state = (reg->ODR >> pin) & 0x1;

	if (state)		// reset
		reg->BSRR = 0x10000 << pin;
	else			// set
		reg->BSRR = 1 << pin;

	return 0;
}

/*
 * Read the state on a GPIO pin
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t GPIO_read(gpio_port port, gpio_pin pin, uint8_t *saved)
{
	if (GPIO_chk_inv(port, pin))
		return 1;

	GPIO_regs *reg = (GPIO_regs *) (gpio_lookup_table[port].enr);

	// Check if the pin is set as input
	if (((reg->MODER >> (1 << pin))) != 0x0)
		return 1;

	*saved = (reg->IDR >> pin) & 0x1;

	return 0;
}

/*
 * Change the mode of a pin
 *
 * gpio_mode: GPIO mode (See gpio.h)
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t GPIO_set_mode(gpio_port port, gpio_pin pin, gpio_mode mode)
{
	if (mode > GPIO_analog || GPIO_chk_inv(port, pin))
		return 1;

	GPIO_regs *reg = (GPIO_regs *) (gpio_lookup_table[port].enr);

	reg->MODER &= ~(0x3 << (1 << pin));
	reg->MODER |= mode << (1 << pin);

	return 0;
}

/*
 * Lock the GPIO registers for some pin
 *
 * NOTE: The lock doesn't apply to GPIOx_BSRR, GPIOx_ODR, and GPIOx_IDR
 * NOTE: This action cannot be undone. The lock is removed only
 * 	 at reset
 */
uint8_t GPIO_lock(gpio_port port, gpio_pin pin)
{
	if (GPIO_chk_inv(port, pin))
		return 1;
	if (GPIO_is_locked(port, pin) == 1)
		return 0;

	GPIO_regs *reg = (GPIO_regs *) (gpio_lookup_table[port].enr);

	uint32_t value = 0x10000 | (1 << pin);

	reg->LCKR = value;
	reg->LCKR = 1 << pin;
	reg->LCKR = value;
	// Reading is part of the sequence
	// `value` is not used anymore
	value = reg->LCKR;

	if (!((reg->LCKR >> 16) & 0x1))
		return 1;

	return 0;
}

/*
 * Return pin lock status. 1 if locked, 0 if unlocked, 255 otherwise
 */
uint8_t GPIO_is_locked(gpio_port port, gpio_pin pin)
{
	if (GPIO_chk_inv(port, pin))
		return 255;

	GPIO_regs *reg = (GPIO_regs *) (gpio_lookup_table[port].enr);

	return ((reg->LCKR >> pin) & 0x1);
}

/* Helper
 *
 * Check if the port/pin combination is sane
 *
 * Return 0 upon success and 1 otherwise
 */
static uint8_t GPIO_chk_inv(gpio_port port, gpio_pin pin)
{
	if (pin > GPIO_PIN15 || port > GPIO_PORTC
	    || (port == GPIO_PORTC && (pin > 15 || pin < 13)))
		return 1;
	return 0;
}
