/*
 * ARM Cortex-M4 specific source
 * Date: Sun May 10 06:56:13 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#include <core/cpu.h>

/*
 * Set PRIMASK to disable interrupts
 */
void CPU_disable_interrupts(void)
{
	// The `CPSID i` instruction is equivalent to writing a 1 into PRIMASK.
	__asm__ volatile ("CPSID i");
}

/*
 * Clear PRIMASK to enable interrupts
 */
void CPU_enable_interrupts(void)
{
	// The `CPSIE i` instruction is equivalent to writing a 0 into PRIMASK.
	__asm__ volatile ("CPSIE i");
}

/*
 * Write the BASEPRI register
 */
void CPU_write_BASEPRI(uint8_t pri)
{
	__asm__ volatile ("MSR BASEPRI, %0"::"r" (pri)
			  :"memory");
}

/*
 * Clear BASEPRI
 */
void CPU_clear_BASEPRI(void)
{
	__asm__ volatile ("MSR BASEPRI, %0"::"r" (0)
			  :"memory");
}

/*
 * Read BASEPRI
 */
uint8_t CPU_read_BASEPRI(void)
{
	uint8_t read;
	__asm__ volatile ("MRS %0, BASEPRI":"=r" (read)
			  ::"memory");
	return read;
}

/*
 * Write FAULTMASK (Disable all interrupts and HardFault)
 */
void CPU_enable_FAULTMASK(void)
{
	// The CPSIE f instruction is equivalent to writing a 0 into FAULTMASK.
	__asm__ volatile ("CPSIE f");
}

/*
 * Clear FAULTMASK (Enable all interrupts and HardFault)
 */
void CPU_disable_FAULTMASK(void)
{
	// The CPSID f instruction is equivalent to writing a 1 into FAULTMASK.
	__asm__ volatile ("CPSID f");
}
