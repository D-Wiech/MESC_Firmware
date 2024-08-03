/*
 * WESC_foc.c
 *
 *  Created on: Aug 2, 2024
 *      Author: Daniel
 */


#include "WESC_header.h"

WESC_motor_struct mtr[1];

void WESC_PWM_handler(WESC_motor_struct *motor){
	uint32_t Cycles = WESC_hardware_get_cycle_counter(motor);

	motor->Stats.PWM_handler_cycles = WESC_hardware_get_cycle_counter(motor) - Cycles;
}

void WESC_ADC_handler(WESC_motor_struct *motor){
	uint32_t Cycles = WESC_hardware_get_cycle_counter(motor);

	motor->Stats.ADC_handler_cycles = WESC_hardware_get_cycle_counter(motor) - Cycles;
}

void WESC_SLOWLoop_handler(WESC_motor_struct *motor){
	uint32_t Cycles = WESC_hardware_get_cycle_counter(motor);

	motor->Stats.SlowLoop_handler_cycles = WESC_hardware_get_cycle_counter(motor) - Cycles;
}

void WESC_init(WESC_motor_struct *motor){
	WESC_init_motorparam(motor);

	WESC_init_hardware_param(motor);


}
