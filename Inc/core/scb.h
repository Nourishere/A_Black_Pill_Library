/*
 * SCB (System Control Block) header
 * Date: Fri May 8 10:00:00 PM EEST 2026
 * Author: Nour Nawar <nournawar5@gmail.com>
 */

#ifndef SCB_H
#define SCB_H

#include <stdint.h>

typedef struct {
	volatile uint32_t *const CPUID;	// CPU ID code
	volatile uint32_t *const ICSR;	// Interrupt Control and Status Register
	volatile uint32_t *const VTOR;	// Vector Table Offset Register
	volatile uint32_t *const AIRCR;	// Application Interrupt / Reset Control Register
	volatile uint32_t *const SCR;	// System Control Register
	volatile uint32_t *const CCR;	// Configuration Control Register
	volatile uint32_t SHP[12];	// System Handler Priority Registers
	volatile uint32_t *const SHCSR;	// System Handle Control and Status Register
	volatile uint32_t *const CFSR;	// Configurable Fault Status Register
	volatile uint32_t *const HFSR;	// HardFault Status Register
	volatile uint32_t *const DFSR;	// Debug Fault Status Register
	volatile uint32_t *const MMFAR;	// MemoryManage Fault Address Register
	volatile uint32_t *const BFAR;	// Bus Fault Address Register
	volatile uint32_t *const AFSR;	// Auxiliary Fault Status Register
	volatile uint32_t PFR[2];	// Processor Feature Registers
	volatile uint32_t *const DFR;	// Debug Feature Register
	volatile uint32_t *const AFR;	// Auxiliary Feature Register
	volatile uint32_t MMFR[4];	// Memory Model Feature Registers
	volatile uint32_t ISAR[5];	// Instruction Set Attributes Register
	volatile uint32_t *const CPACR;	// Co-processor Access Control Register
} SCB_t;

#define SCB ((SCB_t *) 0xE000ED00)

/*
 * Set the interrupt priority group
 *
 * Priority group specifies the split between the preempt priority
 * bits and the sub priority bits.
 *
 * priogroup: can logically take value from 0-7
 * 			  0 means 7 bits are set for preempt and 1 is set for sub
 *			  7 means 1 bit is set for preempt and 7 are set for sub
 *
 * Return nothing
 *
 */
void SCB_set_priority_grouping(uint8_t prigroup);

/*
 * Read the AIRCR and get the priority grouping
 *
 * Return is the least 3 bits in the uint32_t 
 */
uint8_t SCB_get_priority_grouping(void);

/*
 * Encode an interrupt priority given a group/sub priorities
 *
 * prigroup: 3 bit priority group 
 * groupri: group priority
 * subpri: sub priority
 * dev_pri: device-specific total priority levels 
 * 		   (Cortex-M4 allows 3-8 possible levels)
 *
 * Return the full 8 bit priority
 *
 */
uint8_t SCB_encode_priority(uint8_t prigroup, uint8_t groupri, uint8_t subpri,
			    uint8_t dev_pri);

/*
 * Decode an interrupt priority into group/sub priorities
 *
 * priority: Full 8 bit priority
 * prigroup: 3 bit priority group
 * groupri: A pointer to store the decoded group priority
 * subpri: A pointer to store the decoded sub-priority
 * dev_pri: device-specific total priority levels 
 * 		   (Cortex-M4 allows 3-8 possible levels)
 *
 * Return nothing
 *
 */
void SCB_decode_priority(uint8_t priority, uint8_t prigroup, uint8_t * groupri,
			 uint8_t * subpri, uint8_t dev_pri);

#endif				// SCB_H
