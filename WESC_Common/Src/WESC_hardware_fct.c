/*
 * WESC_hardware_fct.c
 *
 *  Created on: Aug 2, 2024
 *      Author: Daniel
 */

#include "WESC_header.h"

//Debug
#define DEMCR_TRCENA    0x01000000
#define DEMCR           (*((volatile uint32_t *)0xE000EDFC))
#define DWT_CTRL        (*(volatile uint32_t *)0xe0001000)
#define CYCCNTENA       (1<<0)
#define DWT_CYCCNT      ((volatile uint32_t *)0xE0001004)
#define CPU_CYCLES      *DWT_CYCCNT

void WESC_hardware_start_cycle_counter(WESC_motor_struct *motor){
	//enable cycle counter
	DEMCR |= DEMCR_TRCENA;
	DWT_CTRL |= CYCCNTENA;
}

uint32_t WESC_hardware_get_cycle_counter(WESC_motor_struct *motor){
	return CPU_CYCLES;
}

void WESC_init_hardware_param(WESC_motor_struct *motor){
	//Load the Hardware parameter
}
