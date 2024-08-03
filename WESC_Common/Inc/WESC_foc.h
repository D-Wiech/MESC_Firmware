/*
 * WESC_foc.h
 *
 *  Created on: Aug 2, 2024
 *      Author: Daniel
 */

#ifndef INC_WESC_FOC_H_
#define INC_WESC_FOC_H_

#include "main.h"
#include "WESC_structs.h"

extern WESC_motor_struct mtr[1];

void WESC_PWM_handler(WESC_motor_struct *motor);

void WESC_ADC_handler(WESC_motor_struct *motor);

void WESC_SLOWLoop_handler(WESC_motor_struct *motor);

void WESC_init(WESC_motor_struct *motor);

#endif /* INC_WESC_FOC_H_ */
