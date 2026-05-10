/*
 * System Control Block (SCB) source
 * Date: Sun May 10 03:33:30 PM EEST 2026
 * Author: Nour Nawar (nournawar5@gmail.com)
 */

#include <core/scb.h>

/*
 * Set the interrupt priority group
 *
 * Priority group specifies the split between the preempt priority
 * bits and the sub-priority bits.
 *
 * priogroup: can logically take value from 0-7
 * 			  0 means 7 bits are set for preempt and 1 is set for sub
 *			  7 means 1 bit is set for preempt and 7 are set for sub
 *
 * Return nothing
 *
 */
void SCB_set_priority_grouping(uint8_t prigroup)
{
	uint32_t reg = *SCB->AIRCR;

	reg &= ~((0xFFFFU << 16) | (0x07 << 8));

	// Write 0x05FA to the upper half word to access the AIRCR
	// and also write the priority group
	reg |= (((prigroup & 0x07) << 8) | (0x05FAU << 16));

	*SCB->AIRCR = reg;
}

/*
 * Read the AIRCR and get the priority grouping
 *
 * Return is the least 3 bits in the uint8_t 
 */
uint8_t SCB_get_priority_grouping(void)
{
	return ((*SCB->AIRCR >> 8) & 0x07);
}

/*
 * Encode an interrupt priority given a group/sub priorities
 *
 * prigroup: 3 bit priority group 
 * groupri: group priority
 * subpri: sub priority
 * devpri: device-specific total priority levels 
 * 		   (Cortex-M4 allows 3-8 possible levels)
 *
 * Return the full 8 bit priority
 *
 */
uint8_t SCB_encode_priority(uint8_t prigroup, uint8_t groupri, uint8_t subpri,
			    uint8_t dev_pri)
{
	uint8_t groupbits, subits, groupmask, submask;

	// Calculate the number of group/sub bits
	subits = (prigroup <= dev_pri) ? 0 : dev_pri - (7U - prigroup);
	groupbits = dev_pri - subits;

	groupmask = (1U << groupbits) - 1;
	submask = (1U << subits) - 1;

	return (((groupri & groupmask) << subits) | (subpri & submask)) << (8U -
									    dev_pri);
}

/*
 * Decode an interrupt priority into group/sub priorities
 *
 * priority: Full 8 bit priority
 * prigroup: 3 bit priority group
 * groupri: A pointer to store the decoded group priority
 * subpri: A pointer to store the decoded sub-priority
 * devpri: device-specific total priority levels 
 * 		   (Cortex-M4 allows 3-8 possible levels)
 *
 * Return nothing
 *
 */
void SCB_decode_priority(uint8_t priority, uint8_t prigroup, uint8_t *groupri,
			 uint8_t *subpri, uint8_t dev_pri)
{
	uint8_t groupbits, subits, groupmask, submask;

	subits = (prigroup <= dev_pri) ? 0 : dev_pri - (7U - prigroup);
	groupbits = dev_pri - subits;

	groupmask = (1U << groupbits) - 1;
	submask = (1U << subits) - 1;

	priority >>= (8U - dev_pri);
	*groupri = (priority >> subits) & groupmask;
	*subpri = priority & submask;
}
