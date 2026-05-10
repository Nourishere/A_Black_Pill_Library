/*
 * ARM Cortex-M4 specific header
 * Date: Fri May 8 10:31:40 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#ifndef CPU_H
#define CPU_H

#include <stdint.h>

/*
 * Set PRIMASK to disable interrupts
 */
void CPU_disable_interrupts(void);

/*
 * Clear PRIMASK to enable interrupts
 */
void CPU_enable_interrupts(void);

/*
 * Write the BASEPRI register
 */
void CPU_write_BASEPRI(uint8_t pri);

/*
 * Clear BASEPRI
 */
void CPU_clear_BASEPRI(void);

/*
 * Read BASEPRI
 */
uint8_t CPU_read_BASEPRI(void);

/*
 * Write FAULTMASK (Disable all interrupts and HardFault)
 */
void CPU_enable_FAULTMASK(void);

/*
 * Clear FAULTMASK (Enable all interrupts and HardFault)
 */
void CPU_disable_FAULTMASK(void);

#endif				// CPU_H
