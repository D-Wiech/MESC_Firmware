/*
 * WESC_structs.h
 *
 *  Created on: Aug 2, 2024
 *      Author: Daniel
 */
/*
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 David Molony.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 ******************************************************************************
 *In addition to the usual 3 BSD clauses, it is explicitly noted that you
 *do NOT have the right to take sections of this code for other projects
 *without attribution and credit to the source. Specifically, if you copy into
 *copyleft licenced code without attribution and retention of the permissive BSD
 *3 clause licence, you grant a perpetual licence to do the same regarding turning sections of your code
 *permissive, and lose any rights to use of this code previously granted or assumed.
 *
 *This code is intended to remain permissively licensed wherever it goes,
 *maintaining the freedom to distribute compiled binaries WITHOUT a requirement to supply source.
 *
 *This is to ensure this code can at any point be used commercially, on products that may require
 *such restriction to meet regulatory requirements, or to avoid damage to hardware, or to ensure
 *warranties can reasonably be honoured.
 ******************************************************************************
  Wile all code is written by Daniel Wiechmann, some code may be based on the MESC Project from David Molony
*/

#ifndef INC_WESC_STRUCTS_H_
#define INC_WESC_STRUCTS_H_

#include "main.h"
//#include "WESC_header.h"

//Encoder specific
#include "MA732_driver.h"


typedef enum {
	MOTOR_TRACKING,
	MOTOR_RUNNING,
	MOTOR_ERROR,
	MOTOR_MEASURING,
	MOTOR_INITIALISING,
	MOTOR_FIXED_VOLTAGE,
} motor_state_e;

typedef enum {
	SENSOR_OPENLOOP,
	SENSOR_SENSORLESS,
	SENSOR_HFI,
	SENSOR_HALL,
	SENSOR_ABSOLUTE,
} motor_sensor;

typedef enum {
	CONTROL_TORQUE,
	CONTROL_RPM,
	FIXED_VOLTAGE,
} motor_controllstyle;

typedef struct {
  float dp_current_final[10];
} motor_encoder_e;

typedef struct {
  float dp_current_final[10];
} motor_startup_encoder_e;

typedef struct {
  float Id;
  float Id_filtered;
  float Iq;
  float Iq_filtered;
  float filter_factor;
} motor_Idq;

typedef struct {
  float Ia;
  float Ib;
} motor_Iab;

typedef struct {
  float Vd;
  float Vq;
} motor_Vdq;

typedef struct {
  float Va;
  float Vb;
} motor_Vab;

typedef struct {
  float Sin;
  float Cos;
} motor_SinCos;

typedef struct {
float inverter_Output_U;
float inverter_Output_V;
float inverter_Output_W;
} motor_inverter_output;

typedef struct {
	uint32_t angle;
	float integral;
	float error;
	float eHz;
	float filterd_eHz;
	float e_w;
	float ki;
	float kp;
} motor_generic_PLL;

typedef struct {
  float Id_int_err;
  float Iq_int_err;
} motor_Idq_error_integral;

typedef struct {
  float Id_err;
  float Iq_err;
} motor_Idq_error;

typedef struct {
  float Id_p;
  float Id_i;
  float Iq_p;
  float Iq_i;
} motor_Idq_pi_gain;

typedef struct {
	uint16_t FOC_Angle;
	float FOC_Voltage_lim;//Vbus * 1/sqrt(3) to avoid overmodulation
	float FOC_Absolute_Voltage;
	float FOC_Vd;
} motor_FOC_limits;

typedef struct {
  float power_q;
  float power_d;
  float power;
} motor_power;

typedef struct {
	motor_Idq Idq;
	motor_Iab Iab;
	motor_Vdq Vdq;
	motor_Vab Vab;
	motor_power pwr;
	motor_inverter_output inv_out;
	motor_SinCos SinCos;
	motor_Idq_error_integral integral;
	motor_Idq_error error;
	motor_Idq_pi_gain pi;
	motor_FOC_limits lim;
	uint16_t FOC_Angle;
} motor_foc_s;

typedef struct {
  float Iu;
  float Iv;
  float Iw;
  float Vbus;
  float Uu;
  float Uv;
  float Uw;
} motor_Converted_val;

typedef struct {
	//Everything needed for the absolute encoder
	MA732_handler abs_encoder;
	uint32_t angle_offset;
	uint16_t angle_direction;// Value 0 or 1
	uint16_t absolute_angle;
} motor_absolute_encoder;

typedef struct {
	//Everything needed for Openloop "encoder"
	float openloop_rpm;
	uint16_t openloop_speed_angle_add;
} motor_openloop_encoder;

typedef struct {
	motor_absolute_encoder absolute;
	motor_openloop_encoder openloop;
	uint32_t full_rotations;

} motor_observer_s;

typedef struct {
  float internal_current_target_q;
  float internal_current_target_d;
  float external_current_target;
  float internal_rpm_target;
  float internal_eHz_target;
  float external_rpm_target;
} motor_input_s;

typedef struct {
  float PWM_U_val;
  float PWM_V_val;
  float PWM_W_val;
  uint8_t inverter_output_enable;
} motor_output_s;

typedef struct {
  uint32_t PWM_handler_cycles;
  uint32_t ADC_handler_cycles;
  uint32_t SlowLoop_handler_cycles;
} WESC_stats_s;

typedef struct {
	uint32_t cal_offset_u;
	uint32_t cal_offset_v;
	uint32_t cal_offset_w;
	uint32_t MAX_ADC_val;
	uint32_t Over_current_limit;

	uint32_t Phase_U;
	uint32_t Phase_V;
	uint32_t Phase_W;

	float Conversion_factor;
} WESC_current_ADC;

typedef struct {
	uint32_t max_voltage;
	uint32_t min_voltage;
	uint32_t MAX_ADC_val;

	uint32_t Phase_U;
	uint32_t Phase_V;
	uint32_t Phase_W;
	uint32_t Vbus;

	float Conversion_factor;
} WESC_voltage_ADC;

typedef struct {
	float Control_Frequency;
	float Control_Period;
	float PWM_Frequency;
	float PWM_Periode;
	float Slow_loop_Frequency;
	float Slow_loop_Periode;
} WESC_hardware_timings;

typedef struct {
	uint16_t Deadtime_offset;
	float pwm_factor;
} WESC_hardware_Deadtime;

typedef struct {
	WESC_current_ADC Current_ADC;
	WESC_voltage_ADC Voltage_ADC;
	WESC_hardware_timings timings;
	WESC_hardware_Deadtime dead_time;
} WESC_hardware;

typedef struct {
	float R;
	float Ld;
	float Lq;
	float max_rpm;
	float max_phase_current;
	float max_power;
	float Pole_pairs;
	float Kv_const;
	float CURRENT_BW;
	int motor_direction_factor;// +1 or -1 indication for motor direction
	float overpower_duration; //how long the maximum current/power setting can be ignored
	float overpower_break_duration; //how long the break between overpower events should be
	float max_overpower_percentage; // the maximum allowed overpower
	float max_overpower;
	float max_overcurrent_percentage; // the maximum allowed overcurrent
	float max_overcurrent;
} WESC_motor_parameter;

typedef struct {
	float Voltage;
} WESC_fixed_voltage_controller;

typedef struct {
	float error;
	float kp;
	float ki;
	float integrator;
} WESC_speed_controller_parameters;

typedef struct {
	uint32_t lock_status; // 0=unlocked | everything else equals locked
	uint32_t lock_time; //Stores how long the lock should last
	float max_allowed_current_input;
	float max_allowed_rpm_input;
} motor_safety_lock;

typedef struct{
	motor_safety_lock lock;
	motor_state_e MotorState;
	motor_encoder_e MotorSensorMode;
	motor_startup_encoder_e SLStartupSensor;
	motor_foc_s FOC;//FOC variables
	motor_observer_s Observer;// Observer Variables
	motor_input_s Input;//motor target rpm or current
	motor_output_s Output;//PWM percentage and more
	motor_Converted_val Con_val;
	motor_sensor sensor_type;
	motor_controllstyle control;
	WESC_fixed_voltage_controller fixedVoltage;
	WESC_motor_parameter motor_para;
	motor_generic_PLL PLL;
	WESC_speed_controller_parameters speed_con;

	WESC_stats_s Stats;//Contains infomation like for eaxample execution cycles for an handler

	//MCU dependend stuff
	WESC_hardware hardware;
	TIM_HandleTypeDef *mtimer; //3 phase PWM timer
	TIM_HandleTypeDef *stimer; //Timer that services the slowloop
}WESC_motor_struct;


#endif /* INC_WESC_STRUCTS_H_ */
