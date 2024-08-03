/*
 * WESC_hardware_fct.h
 *
 *  Created on: Aug 2, 2024
 *      Author: Daniel
 */

#ifndef INC_WESC_HARDWARE_FCT_H_
#define INC_WESC_HARDWARE_FCT_H_

#include "main.h"
#include "WESC_structs.h"

void WESC_hardware_start_cycle_counter(WESC_motor_struct *motor);

uint32_t WESC_hardware_get_cycle_counter(WESC_motor_struct *motor);

void WESC_init_hardware_param(WESC_motor_struct *motor);

#endif /* INC_WESC_HARDWARE_FCT_H_ */
