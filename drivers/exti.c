/*
 * External Interrupt (EXTI) controller source
 * Date: Thu May 14 03:17:57 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#include <drivers/exti.h>

uint8_t EXTI_check_EXTIn(EXTIn_t line);
IRQn_t EXTI_exti2irq(EXTIn_t line);

/*
 * Initialize an external interrupt line (see enum `EXTn_t`)
 *
 * NOTE: An external voltage switch (rise, fall, or both)
 * 		triggers an interrupt on that line's IRQ.
 * Sequence
 * - Disable
 * - Clear registers
 * - Unmask line
 * - Set priority
 * - Re-enable if necessary
 *
 * line: EXTI line to set
 * edge: (use enum `edge_t`)
 * priority: Interrupt priority
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t EXTI_set_line(EXTIn_t line, edge_t edge, uint8_t priority)
{
	if (EXTI_check_EXTIn(line) || edge > both)
		return 1;

	// Unmask the line before initialization
	uint32_t state = ((EXTI_IMR >> line) & 1) ? 1 : 0;

	// Reset the registers
	EXTI_IMR = 0x00000000U;
	EXTI_EMR = 0x00000000U;
	EXTI_RTSR = 0x00000000U;
	EXTI_FTSR = 0x00000000U;
	EXTI_SWIER = 0x00000000U;
	// Writing one clears the register (C1W)
	// Reserved bits should't be set
	EXTI_PR = 0x0087FFFFU;

	switch (edge) {
	case (rising):
		EXTI_RTSR |= 1 << line;
		break;
	case (falling):
		EXTI_FTSR |= 1 << line;
		break;
	case (both):
		EXTI_RTSR |= 1 << line;
		EXTI_FTSR |= 1 << line;
		break;
	default:
		return 1;
	}

	IRQn_t irq_line = EXTI_exti2irq(line);
	NVIC_set_priority(irq_line, priority);

	if (state)
		EXTI_IMR |= 1 << line;

	return 0;
}

/*
 * Initialize an external interrupt event (see enum `EXTIn_t`)
 *
 * NOTE: An external voltage switch (rise, fall, or both)
 * 		triggers a special event (see EXTI in enum `IRQn_t`).
 *
 * Sequence
 * - Disable
 * - Clear registers
 * - Unmask event line
 * - Set priority
 * - Re-enable if necessary
 *
 * line: EXTI line to set
 * edge: (use enum `edge_t`)
 * priority: Interrupt priority
 *
 * Return 0 upon success and 1 otherwise
 */
uint8_t EXTI_set_event(EXTIn_t line, edge_t edge, uint8_t priority)
{
	if (EXTI_check_EXTIn(line) || edge > both)
		return 1;

	// Unmask the line before initialization
	uint32_t state = ((EXTI_EMR >> line) & 1) ? 1 : 0;

	// Reset the registers
	EXTI_IMR = 0x00000000U;
	EXTI_EMR = 0x00000000U;
	EXTI_RTSR = 0x00000000U;
	EXTI_FTSR = 0x00000000U;
	EXTI_SWIER = 0x00000000U;
	// Writing one clears the register (C1W)
	// Reserved bits should't be written
	EXTI_PR = 0x0087FFFFU;

	switch (edge) {
	case (rising):
		EXTI_RTSR |= 1 << line;
		break;
	case (falling):
		EXTI_FTSR |= 1 << line;
		break;
	case (both):
		EXTI_RTSR |= 1 << line;
		EXTI_FTSR |= 1 << line;
		break;
	default:
		return 1;
	}

	EXTI_EMR |= 1 << line;

	IRQn_t irq_line = EXTI_exti2irq(line);
	NVIC_set_priority(irq_line, priority);

	if (state)
		EXTI_EMR |= 1 << line;

	return 0;
}

/*
 * Enable IRQ requests on an EXTI line
 * When triggered, the IRQ line is sent to the NVIC
 *
 * line: EXTI line number to enable
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t EXTI_enable_line(EXTIn_t line)
{
	if (EXTI_check_EXTIn(line))
		return 1;

	EXTI_IMR |= 1 << line;

	return 0;
}

/*
 * Enable event requests on an EXTI line
 *
 * line: EXTI line number to enable
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t EXTI_enable_event(EXTIn_t line)
{
	if (EXTI_check_EXTIn(line))
		return 1;

	EXTI_EMR |= 1 << line;

	return 0;
}

/*
 * Disable an interrupt line
 *
 * line: EXTI line number to disable
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t EXTI_disable_line(EXTIn_t line)
{
	if (EXTI_check_EXTIn(line))
		return 1;

	EXTI_IMR &= ~(1 << line);

	return 0;
}

/*
 * Disable an interrupt event
 *
 * line: EXTI line number to disable
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t EXTI_disable_event(EXTIn_t line)
{
	if (EXTI_check_EXTIn(line))
		return 1;

	EXTI_EMR &= ~(1 << line);

	return 0;
}

/*
 * Trigger a software interrupt on an EXTI line
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t EXTI_trigger_interrupt(EXTIn_t line)
{
	if (EXTI_check_EXTIn(line))
		return 1;

	EXTI_SWIER |= 1 << line;
	return 0;
}

/*
 * Clear the pending state on an EXTI line
 *
 * Return 0 upon success and 1 otherwise
 *
 */
uint8_t EXTI_clear_pending(EXTIn_t line)
{
	if (EXTI_check_EXTIn(line))
		return 1;

	// This register is a C1W (Write 1 to clear)
	// Writing 0s to this register doesn't change state
	EXTI_PR = 1 << line;

	return 0;
}

/* Helper
 * Check for unvalid line values
 *
 * NOTE: lines 19 and 20 are not implemented on the current device
 *
 * Return 0 if line is valid and 1 otherwise
 *
 */
uint8_t EXTI_check_EXTIn(EXTIn_t line)
{
	if (line == EXTI_19 || line == EXTI_20 || line > EXTI_22)
		return 1;
	return 0;
}

/* Helper
 * Convert an EXTIn_t type to an IRQn_t type
 * Used for functions that need to call core/nvic
 *
 * Return the IRQn_t type. Does no checking.
 *
 */
IRQn_t EXTI_exti2irq(EXTIn_t line)
{
	IRQn_t irq_line;
	switch (line) {
	case (EXTI_1):
		irq_line = EXTI1;
		break;
	case (EXTI_2):
		irq_line = EXTI2;
		break;
	case (EXTI_3):
		irq_line = EXTI3;
		break;
	case (EXTI_4):
		irq_line = EXTI4;
		break;
	case (EXTI_5):
		irq_line = EXTI5;
		break;
	case (EXTI_6):
		irq_line = EXTI6;
		break;
	case (EXTI_7):
		irq_line = EXTI7;
		break;
	case (EXTI_8):
		irq_line = EXTI8;
		break;
	case (EXTI_9):
		irq_line = EXTI9;
		break;
	case (EXTI_10):
		irq_line = EXTI10;
		break;
	case (EXTI_11):
		irq_line = EXTI11;
		break;
	case (EXTI_12):
		irq_line = EXTI12;
		break;
	case (EXTI_13):
		irq_line = EXTI13;
		break;
	case (EXTI_14):
		irq_line = EXTI14;
		break;
	case (EXTI_15):
		irq_line = EXTI15;
		break;
	case (EXTI_16):
		irq_line = EXTI16;
		break;
	case (EXTI_17):
		irq_line = EXTI17;
		break;
	case (EXTI_18):
		irq_line = EXTI18;
		break;
	case (EXTI_21):
		irq_line = EXTI21;
		break;
	case (EXTI_22):
		irq_line = EXTI22;
		break;
	default:
		return -1;
	}
	return irq_line;
}
