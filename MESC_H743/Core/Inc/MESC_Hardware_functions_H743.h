/*
 * MESC_Hardware_functions_H743.h
 *
 *  Created on: Aug 4, 2024
 *      Author: Daniel
 */

#ifndef INC_MESC_HARDWARE_FUNCTIONS_H743_H_
#define INC_MESC_HARDWARE_FUNCTIONS_H743_H_

#include "MESCfoc.h"

//Debug
#define DEMCR_TRCENA    0x01000000
#define DEMCR           (*((volatile uint32_t *)0xE000EDFC))
#define DWT_CTRL        (*(volatile uint32_t *)0xe0001000)
#define CYCCNTENA       (1<<0)
#define DWT_CYCCNT      ((volatile uint32_t *)0xE0001004)
#define CPU_CYCLES      *DWT_CYCCNT

void Hardware_Debugger_cycle_setup(MESC_motor_typedef *_motor);

#endif /* INC_MESC_HARDWARE_FUNCTIONS_H743_H_ */
