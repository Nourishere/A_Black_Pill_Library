/*
 * GPIO.c
 *
 *  Created on: Dec 4, 2025
 *      Author: nour
 *  Brief: Low Level GPIO driver.
 */
#include <drivers/gpio.h>

/* Check invalid port-pin arguments */
static uint32_t GPIO_chk_inv(GPIO_t* gpio);

GPIO_t GPIO_default (uint8_t pin, uint8_t port)
{
	GPIO_t def;
	def.pin = pin;
	def.port = port;
	uint32_t ret = GPIO_chk_inv(&def);
	if(ret != __ROK){
		GPIO_t neg;
		return neg;
	}

	def.out_speed = HIGH;
	def.out_type = PP;
	def.pp_res = 0;

	return def;
}

uint32_t GPIO_init (GPIO_t* gpio)		
{
	uint32_t ret = GPIO_chk_inv(gpio);
	if(ret != __ROK)
		return ret;
	switch(gpio->port){
		case (PORTA):
			GPIOA->MODER|=(gpio->mode) << (gpio->pin << 1);
			GPIOA->OTYPER|=(gpio->out_type) << (gpio->pin);
			GPIOA->OSPEEDR|=(gpio->out_speed) << (gpio->pin << 1);
			GPIOA->OPUPDR|=(gpio->pp_res) << (gpio->pin << 1);
			break;
		case (PORTB):
			GPIOB->MODER|=(gpio->mode) << (gpio->pin << 1);
			GPIOB->OTYPER|=(gpio->out_type) << (gpio->pin);
			GPIOB->OSPEEDR|=(gpio->out_speed) << (gpio->pin << 1);
			GPIOB->OPUPDR|=(gpio->pp_res) << (gpio->pin << 1);
			break;
		case (PORTC):
			GPIOC->MODER|=(gpio->mode) << (gpio->pin << 1);
			GPIOC->OTYPER|=(gpio->out_type) << (gpio->pin);
			GPIOC->OSPEEDR|=(gpio->out_speed) << (gpio->pin << 1);
			GPIOC->OPUPDR|=(gpio->pp_res) << (gpio->pin << 1);
			break;
		default:
			return __RNOK;
	}	
	return __ROK;
}

uint32_t GPIO_set (GPIO_t* gpio)
{
	uint32_t ret = GPIO_chk_inv(gpio);
	if(ret != __ROK)
		return ret;
	if(gpio->mode != OUT)
		return __RNOK;
	switch(gpio->port){
		case (PORTA):
			GPIOA->ODR|=(1 << (gpio->pin));
			break;
		case (PORTB):
			GPIOB->ODR|=(1 << (gpio->pin));
			break;
		case (PORTC):
			GPIOC->ODR|=(1 << (gpio->pin));
			break;
		default:
			return __RNOK;
	}
	return __ROK;
}

uint32_t GPIO_clear (GPIO_t* gpio)
{
	uint32_t ret = GPIO_chk_inv(gpio);
	if(ret != __ROK)
		return ret;
	if(gpio->mode != OUT)
		return __RNOK;
	switch(gpio->port){
		case (PORTA):
			GPIOA->ODR&=~(1 << (gpio->pin));
			break;
		case (PORTB):
			GPIOB->ODR&=~(1 << (gpio->pin));
			break;
		case (PORTC):
			GPIOC->ODR&=~(1 << (gpio->pin));
			break;
		default:
			return __RNOK;
	}
	return __ROK;
}

uint32_t GPIO_toggle (GPIO_t* gpio)
{
	uint32_t ret = GPIO_chk_inv(gpio);
	if(ret != __ROK)
		return ret;
	if(gpio->mode != OUT)
		return __RNOK;
	switch(gpio->port){
		case (PORTA):
			GPIOA->ODR^=(1 << (gpio->pin));
			break;
		case (PORTB):
			GPIOB->ODR^=(1 << (gpio->pin));
			break;
		case (PORTC):
			GPIOC->ODR^=(1 << (gpio->pin));
			break;
		default:
			return __RNOK;
	}
	return __ROK;
}

uint32_t GPIO_read (GPIO_t* gpio, uint32_t* saved)
{
	uint32_t ret = GPIO_chk_inv(gpio);
	if(ret != __ROK)
		return ret;
	if(gpio->mode != IN)
		return __RNOK;
	switch(gpio->port){
		case (PORTA):
			*saved = (GPIOA->IDR >> gpio->pin) & 0x01;
			break;
		case (PORTB):
			*saved = (GPIOB->IDR >> gpio->pin) & 0x01;
			break;
		case (PORTC):
			*saved = (GPIOC->IDR >> gpio->pin) & 0x01;
			break;
		default:
			return __RNOK;
	}
	return __ROK;
}


static uint32_t GPIO_chk_inv(GPIO_t* gpio)
{
	if(gpio == 0)
		return __RNULL;
	if(gpio->pin > PIN15 || gpio->pin < PIN0 || gpio->port < PORTA || gpio->port > PORTC ||
	   (gpio-> port == PORTC && (gpio->pin > 15 || gpio->pin < 13)))
		return __RINVAR;
	return __ROK;
}
