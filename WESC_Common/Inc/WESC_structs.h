/*
 * WESC_structs.h
 *
 *  Created on: Aug 2, 2024
 *      Author: Daniel
 */

#ifndef INC_WESC_STRUCTS_H_
#define INC_WESC_STRUCTS_H_

#include "main.h"

typedef struct {
  float dp_current_final[10];
} motor_state_e;

typedef struct {
  float dp_current_final[10];
} motor_encoder_e;

typedef struct {
  float dp_current_final[10];
} motor_startup_encoder_e;

typedef struct {
  float dp_current_final[10];
} motor_control_mode_e;

typedef struct {
  float dp_current_final[10];
} motor_foc_s;

typedef struct {
  float dp_current_final[10];
} motor_observer_s;

typedef struct {
  float internal_current_target;
  float external_current_target;
  float internal_rpm_target;
  float external_rpm_target;
} motor_input_s;

typedef struct {
  float PWM_U_val;
  float PWM_V_val;
  float PWM_W_val;
} motor_output_s;

typedef struct {
  uint32_t PWM_handler_cycles;
  uint32_t ADC_handler_cycles;
  uint32_t SlowLoop_handler_cycles;
} WESC_stats_s;

typedef struct{
	motor_state_e MotorState;
	motor_encoder_e MotorSensorMode;
	motor_startup_encoder_e SLStartupSensor;
	motor_control_mode_e ControlMode;
	motor_foc_s FOC;//FOC variables
	motor_observer_s Observer;// Observer Variables
	motor_input_s Input;//motor target rpm or current
	motor_output_s Output;//PWM percentage and more

	WESC_stats_s Stats;//Contains infomation like for eaxample execution cycles for an handler

	//MCU dependend stuff
	TIM_HandleTypeDef *mtimer; //3 phase PWM timer
	TIM_HandleTypeDef *stimer; //Timer that services the slowloop
}WESC_motor_struct;


#endif /* INC_WESC_STRUCTS_H_ */
