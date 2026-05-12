/*
 * STM32F401-specific interrupt API implementation
 *
 * Date: Tue May 12 03:46:08 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#include <drivers/interrupt.h>

static uint8_t interrupt_check_IRQn(IRQn_t line);

/*
 * Initialize interrupts by doing the following:
 * - Setting the preempt/subgroup.
 * - Disabling interrupts completely.
 * - Clearning priority and pending states.
 *
 * NOTE: Clear interrupt masking after calling this function to 
 * 		ensure interrupt are unmasked.
 *
 * NOTE: This function gives you the possibility to either use
 * 		or ignore sub priorities. It should be called once
 * 		at initialization time.
 *
 * groupbits: The number of preempt priority bits.
 *
 * NOTE: if you wish NOT to use any sub priority, just 
 * 		set groupbits to INTERRUPT_PRIO_BITS.
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t interrupt_init(uint8_t groupbits)
{
	// Set priority grouping
	if (groupbits < 1 || groupbits > INTERRUPT_PRIO_BITS)
		return 1;

	NVIC_disable_all();
	NVIC_clear_all_pending();
	NVIC_reset_all_priorities();

	SCB_set_priority_grouping(7U - groupbits);
	return 0;
}

/*
 * Set the priority of an interrupt line
 *
 * NOTE: The interrupt priority you give is the full 8-bit priority
 *
 * priority: The priority you wish to set 
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t interrupt_set_priority(IRQn_t line, uint8_t priority)
{
	if (interrupt_check_IRQn(line))
		return 1;
	NVIC_set_priority(line, priority);

	return 0;
}

/*
 * Fetch the current priority on an IRQ line
 * 
 * priority: A pointer to store the priority
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t interrupt_get_priority(IRQn_t line, uint8_t *priority)
{
	if (interrupt_check_IRQn(line))
		return 1;
	*priority = NVIC_get_priority(line);

	return 0;
}

/*
 * Set the pending state for an IRQ line
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t interrupt_set_pending(IRQn_t line)
{
	if (interrupt_check_IRQn(line))
		return 1;
	NVIC_set_pending(line);

	return 0;
}

/*
 * Clear the pending state for an IRQ line
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t interrupt_clear_pending(IRQn_t line)
{
	if (interrupt_check_IRQn(line))
		return 1;
	NVIC_clear_pending(line);

	return 0;
}

/*
 * Is the IRQ line pending service? 
 *
 * Return 1 if it is and 0 if is not
 *
 */
uint8_t interrupt_is_pending(IRQn_t line)
{
	if (interrupt_check_IRQn(line))
		return 1;

	return NVIC_is_pending(line);
}

/*
 * Is the IRQ line being served? 
 *
 * Return 1 if it is and 0 if is not
 *
 */
uint8_t interrupt_is_active(IRQn_t line)
{
	if (interrupt_check_IRQn(line))
		return 1;

	return NVIC_is_active(line);
}

/*
 * Enable an IRQ line
 *
 * Return 0 upon success and 1 otherwise 
 *
 */
uint8_t interrupt_enable_IRQ(IRQn_t line)
{
	if (interrupt_check_IRQn(line))
		return 1;
	NVIC_enable_IRQ(line);

	return 0;
}

/*
 * Disable an IRQ line
 *
 * Return 0 upon success and 0 otherwise 
 *
 */
uint8_t interrupt_disable_IRQ(IRQn_t line)
{
	if (interrupt_check_IRQn(line))
		return 1;
	NVIC_disable_IRQ(line);

	return 0;
}

/* Helper
 * Check if the IRQn_t input is valid
 *
 * Return 0 if it is valid and 1 otherwise
 *
 */
static uint8_t interrupt_check_IRQn(IRQn_t line)
{
	if (line > PI4)
		return 1;
	if ((line > 18 && line < 23) || (line == 39) || (line > 42 && line < 47)
	    || (line == 48) || (line > 51 && line < 56) || (line > 60
							    && line < 67)
	    || (line > 73 && line < 81) || (line > 81 && line < 84))
		return 1;
	return 0;
}
