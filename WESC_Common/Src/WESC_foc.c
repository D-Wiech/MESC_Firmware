/*
 * WESC_foc.c
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

#include "WESC_header.h"

#define divide_by_2 0.5f*
#define sqrt3_on_2  0.866025f
const float inv_sqrt_3 = 0.57735026918962576450914878050196f;
#define PI 3.14159265f
#define PI_2 2.f*PI
#define PI_half PI/2.f
#define Angle_base 65536.f

#define PLL_KP 1.5f
#define PLL_KI 0.001f

WESC_motor_struct mtr[1];

void WESC_inverse_park(WESC_motor_struct *motor);
void WESC_inverse_clark(WESC_motor_struct *motor);
void WESC_SVPWM_calc(WESC_motor_struct *motor);
void WESC_SVPWM_calc(WESC_motor_struct *motor);
void WESC_ADC_Current_Conversion(WESC_motor_struct *motor);
void WESC_ErrorHandler(WESC_motor_struct *motor);
void WESC_Clark(WESC_motor_struct *motor);
void WESC_Park(WESC_motor_struct *motor);
void WESC_PLL(WESC_motor_struct* motor);
void WESC_Current_Controller(WESC_motor_struct* motor);
void WESC_ADC_VBUS_Conversion(WESC_motor_struct *motor);
void WESC_get_openloop_angle(WESC_motor_struct *motor);
void WESC_set_openloop_rpm(WESC_motor_struct *motor);
void WESC_set_internal_target(WESC_motor_struct *motor);
void WESC_safety_lock(WESC_motor_struct *motor);
void WESC_enable_safety_lock(WESC_motor_struct *motor);
void WESC_calculate_power(WESC_motor_struct *motor);
void WESC__power_controller(WESC_motor_struct *motor);
void WESC_speed_controller(WESC_motor_struct *motor);
void WESC_filter_iqd(WESC_motor_struct *motor);


void WESC_PWM_handler(WESC_motor_struct *motor){
	uint32_t Cycles = WESC_hardware_get_cycle_counter(motor);
	if (WESC_motortimer_downcounting(motor)) { //Polling the DIR (direction) bit on the motor counter DIR = 1 = downcounting
		//WESC_sin_cos_fast(motor->FOC.FOC_Angle, &motor->FOC.SinCos.Sin, &motor->FOC.SinCos.Cos);
		//WESC_inverse_park(motor);
		//WESC_inverse_clark(motor);
		//WESC_SVPWM_calc(motor);
		//WESC_set_PWM_register(motor);
	}
	if (!(WESC_motortimer_downcounting(motor))) { //Polling the DIR (direction) bit on the motor counter DIR = 0 = upcounting
		//RunHFI(motor);
		WESC_sin_cos_fast(motor->FOC.FOC_Angle, &motor->FOC.SinCos.Sin, &motor->FOC.SinCos.Cos);
		WESC_inverse_park(motor);
		WESC_inverse_clark(motor);
		WESC_SVPWM_calc(motor);
		WESC_set_PWM_register(motor);
	}
	motor->Stats.PWM_handler_cycles = WESC_hardware_get_cycle_counter(motor) - Cycles;
}

void WESC_ADC_handler(WESC_motor_struct *motor){
	uint32_t Cycles = WESC_hardware_get_cycle_counter(motor);
	WESC_get_ADC_current_data(motor);
	WESC_ADC_Current_Conversion(motor);
	WESC_Clark(motor);
	WESC_Park(motor);
	WESC_get_absolute_encoder_value(motor);

	//Execute the Sensor function
	switch(motor->sensor_type){
	case SENSOR_OPENLOOP:
		WESC_get_openloop_angle(motor);
		break;
	case SENSOR_HFI:
		break;
	case SENSOR_HALL:
		break;
	case SENSOR_ABSOLUTE:
		//WESC_get_absolute_encoder_value(motor);
		motor->FOC.FOC_Angle = motor->Observer.absolute.absolute_angle;
		break;
	default:
		break;
	}

	//Control switch case
	switch(motor->MotorState){
	case MOTOR_TRACKING:
		break;
	case MOTOR_RUNNING:
		WESC_Current_Controller(motor);
		break;
	case MOTOR_FIXED_VOLTAGE:
		motor->FOC.Vdq.Vd = 0;
		motor->FOC.Vdq.Vq = motor->fixedVoltage.Voltage;
		break;
	case MOTOR_MEASURING:
		break;
	case MOTOR_INITIALISING:
		//Temporary setting all offsets with a fixed value
		motor->hardware.Current_ADC.cal_offset_u = 32768;
		motor->hardware.Current_ADC.cal_offset_v = 32768;
		motor->hardware.Current_ADC.cal_offset_w = 32768;
		break;
	case MOTOR_ERROR:
	default:
		break;
	}
	//Possible to take the angle delay in account ToDo
	WESC_filter_iqd(motor);
	WESC_PLL(motor);
	motor->Stats.ADC_handler_cycles = WESC_hardware_get_cycle_counter(motor) - Cycles;
}

void WESC_SLOWLoop_handler(WESC_motor_struct *motor){
	uint32_t Cycles = WESC_hardware_get_cycle_counter(motor);
	//order matters
	WESC_get_VBUS_data(motor);
	WESC_ADC_VBUS_Conversion(motor);
	WESC_safety_lock(motor);
	WESC_set_internal_target(motor);
	if((motor->MotorState = MOTOR_RUNNING) && (motor->control == CONTROL_RPM)){
		WESC_speed_controller(motor);
	}
	WESC__power_controller(motor);


	motor->Stats.SlowLoop_handler_cycles = WESC_hardware_get_cycle_counter(motor) - Cycles;
}

void WESC_init(WESC_motor_struct *motor){
	//Temporary always used sensor
	motor->sensor_type = SENSOR_ABSOLUTE;
	motor->MotorState = MOTOR_INITIALISING;
	motor->PLL.ki = PLL_KI;
	motor->PLL.kp = PLL_KP;

	WESC_hardware_start_cycle_counter(motor);
	WESC_init_hardware_param(motor);
	WESC_init_motorparam(motor);
	WESC_init_hardware(motor);


}

void WESC_inverse_park(WESC_motor_struct *motor){
	// Inverse Park transform
	motor->FOC.Vab.Va = motor->FOC.SinCos.Cos * motor->FOC.Vdq.Vd - motor->FOC.SinCos.Sin * motor->FOC.Vdq.Vq;
	motor->FOC.Vab.Vb = motor->FOC.SinCos.Sin * motor->FOC.Vdq.Vd + motor->FOC.SinCos.Cos * motor->FOC.Vdq.Vq;
}

void WESC_inverse_clark(WESC_motor_struct *motor){
	// Inverse Clark transform - power variant
	motor->FOC.inv_out.inverter_Output_U = motor->FOC.Vab.Va;
	motor->FOC.inv_out.inverter_Output_V = -divide_by_2  motor->FOC.inv_out.inverter_Output_U;
	motor->FOC.inv_out.inverter_Output_W = motor->FOC.inv_out.inverter_Output_V - sqrt3_on_2 * motor->FOC.Vab.Vb;
	motor->FOC.inv_out.inverter_Output_V = motor->FOC.inv_out.inverter_Output_V + sqrt3_on_2 * motor->FOC.Vab.Vb;
}

// fast_atan2 based on https://math.stackexchange.com/a/1105038/81278
// Via Odrive project
// https://github.com/odriverobotics/ODrive/blob/master/Firmware/MotorControl/utils.cpp
// This function is MIT licenced, copyright Oskar Weigl/Odrive Robotics
// The origin for Odrive atan2 is public domain. Thanks to Odrive for making
// it easy to borrow.
float minf(float lhs, float rhs) { return (lhs < rhs) ? lhs : rhs; }
float maxf(float lhs, float rhs) { return (lhs > rhs) ? lhs : rhs; }

float fast_atan2(float y, float x) {
  // a := min (|x|, |y|) / max (|x|, |y|)
  float abs_y = fabsf(y);
  float abs_x = fabsf(x);
  // inject FLT_MIN in denominator to avoid division by zero
  float a = minf(abs_x, abs_y) / (maxf(abs_x, abs_y));
  // s := a * a
  float s = a * a;
  // r := ((-0.0464964749 * s + 0.15931422) * s - 0.327622764) * s * a + a
  float r =
      ((-0.0464964749f * s + 0.15931422f) * s - 0.327622764f) * s * a + a;
  // if |y| > |x| then r := 1.57079637 - r
  if (abs_y > abs_x) r = 1.57079637f - r;
  // if x < 0 then r := 3.14159274 - r
  if (x < 0.0f) r = 3.14159274f - r;
  // if y < 0 then r := -r
  if (y < 0.0f) r = -r;

  return r;
}

float fast_inverse_sqrt(float number)
{
	//See: https://en.wikipedia.org/wiki/Fast_inverse_square_root
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
  return y;
}

void FOC_SVPWM_calculation_FT_Modified(WESC_motor_struct *motor)
{
	//Calculates the PWM value (proportional on time) for the half bridges
	//A modified variant of the Flat top space vector implementation
	float inv_Vbus = 1.f / motor->Con_val.Vbus;
	float PWM_U_val = (motor->Con_val.Vbus - motor->FOC.inv_out.inverter_Output_U) * inv_Vbus;
	float PWM_V_val = (motor->Con_val.Vbus - motor->FOC.inv_out.inverter_Output_V) * inv_Vbus;
	float PWM_W_val = (motor->Con_val.Vbus - motor->FOC.inv_out.inverter_Output_W) * inv_Vbus;

	//calculating the min mean from the min and max values for U,V,W
	//finding the min value
	float min = 0; float max = 0;
	min = minf(PWM_U_val, PWM_V_val);
	min = minf(PWM_W_val, min);
	//finding the max value
	max = maxf(PWM_U_val, PWM_V_val);
	max = maxf(PWM_W_val, max);

	float mid = divide_by_2 (max + min);

	motor->Output.PWM_U_val = (PWM_U_val - mid) + 0.5f;
	motor->Output.PWM_V_val = (PWM_V_val - mid) + 0.5f;
	motor->Output.PWM_W_val = (PWM_W_val - mid) + 0.5f;
}

void FOC_SVPWM_calculation_FT(WESC_motor_struct* motor)
{
	//Implements the Flat Top space vector implementation accordind to: https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ApplicationNotes/ApplicationNotes/00002590B.pdf
	/* Not yet finished
	float inv_Vbus = 1.f / motor->Conv_values.Vbus;
	motor->Output.PWM_U_val = motor->FOC_Val.PWM_Periode *(motor->Conv_values.Vbus - motor->FOC_Val.V_r1) * inv_Vbus;
	motor->Output.PWM_V_val = motor->FOC_Val.PWM_Periode *(motor->Conv_values.Vbus - motor->FOC_Val.V_r2) * inv_Vbus;
	motor->Output.PWM_W_val = motor->FOC_Val.PWM_Periode *(motor->Conv_values.Vbus - motor->FOC_Val.V_r3) * inv_Vbus;

	float Vmin =  minf(motor->Output.PWM_U_val , minf(motor->Output.PWM_V_val, motor->Output.PWM_W_val));
	motor->Output.PWM_U_val = motor->Output.PWM_U_val - Vmin;
	motor->Output.PWM_V_val = motor->Output.PWM_V_val - Vmin;
	motor->Output.PWM_W_val = motor->Output.PWM_W_val - Vmin;
	*/
}

void WESC_SVPWM_calc(WESC_motor_struct *motor){
	FOC_SVPWM_calculation_FT_Modified(motor);
	//FOC_SVPWM_calculation_FT(motor);
}



void WESC_ADC_Current_Conversion(WESC_motor_struct *motor){
	//Subtract the ADC offset
	int32_t Phase_U_current = motor->hardware.Current_ADC.Phase_U - motor->hardware.Current_ADC.cal_offset_u;
	int32_t Phase_V_current = motor->hardware.Current_ADC.Phase_V - motor->hardware.Current_ADC.cal_offset_v;
	int32_t Phase_W_current = motor->hardware.Current_ADC.Phase_W - motor->hardware.Current_ADC.cal_offset_w;

	//Check for Overcurrent condition
	if(abs(Phase_U_current) > motor->hardware.Current_ADC.Over_current_limit){ WESC_ErrorHandler(motor);}
	if(abs(Phase_V_current) > motor->hardware.Current_ADC.Over_current_limit){ WESC_ErrorHandler(motor);}
	if(abs(Phase_W_current) > motor->hardware.Current_ADC.Over_current_limit){ WESC_ErrorHandler(motor);}

	//Convert the ADC reading to AMPs
	motor->Con_val.Iu = ((float)Phase_U_current) * motor->hardware.Current_ADC.Conversion_factor;
	motor->Con_val.Iv = ((float)Phase_V_current) * motor->hardware.Current_ADC.Conversion_factor;
	motor->Con_val.Iw = ((float)Phase_W_current) * motor->hardware.Current_ADC.Conversion_factor;
}


void WESC_ADC_VBUS_Conversion(WESC_motor_struct *motor){
	if(motor->hardware.Voltage_ADC.Vbus > motor->hardware.Voltage_ADC.max_voltage){WESC_ErrorHandler(motor);}
	if(motor->hardware.Voltage_ADC.Vbus < motor->hardware.Voltage_ADC.min_voltage){WESC_ErrorHandler(motor);}

	motor->Con_val.Vbus = (float)motor->hardware.Voltage_ADC.Vbus * motor->hardware.Voltage_ADC.Conversion_factor;

	//Also calculte the max modulation Voltage
	motor->FOC.lim.FOC_Voltage_lim = motor->Con_val.Vbus * inv_sqrt_3;
}

void WESC_ErrorHandler(WESC_motor_struct *motor){
	WESC_Disable_PWM(motor);
	motor->Output.inverter_output_enable = 0;
}

void WESC_Clark(WESC_motor_struct *motor){
	//Making the full clark transformation atm
	const float two_thirds = 0.6666666f;
	const float one_third = 0.333333333f;
	motor->FOC.Iab.Ia = two_thirds * motor->Con_val.Iu - (motor->Con_val.Iv + motor->Con_val.Iw) * one_third;
	motor->FOC.Iab.Ib = (motor->Con_val.Iv - motor->Con_val.Iw) * 2.f * inv_sqrt_3;
}

void WESC_Park(WESC_motor_struct *motor){
	//Doing the Park Transformation with the sinus and cosinus values calculated in the PWM Handler
	motor->FOC.Idq.Id = motor->FOC.Iab.Ia * motor->FOC.SinCos.Cos + motor->FOC.Iab.Ib * motor->FOC.SinCos.Sin;
	motor->FOC.Idq.Iq = -(motor->FOC.Iab.Ia * motor->FOC.SinCos.Sin) + motor->FOC.Iab.Ib * motor->FOC.SinCos.Cos;
}

void WESC_PLL(WESC_motor_struct* motor)
{
	const float divide_by_65536 = 0.00001526f;
	//Calculating the new PLL angle
	motor->PLL.angle = motor->PLL.angle + (int16_t)motor->PLL.integral + (int16_t)motor->PLL.error;
	//Calculating the PLL error
	motor->PLL.error = motor->PLL.kp * (int16_t)(motor->FOC.FOC_Angle - (motor->PLL.angle & 0xFFFF));
	//Calculating the integral value
	motor->PLL.integral = motor->PLL.integral + motor->PLL.ki * motor->PLL.error;
	//Calculating the electrical Hz from the Integral value
	motor->PLL.eHz = motor->PLL.integral * motor->hardware.timings.Control_Frequency*divide_by_65536;

	motor->PLL.e_w = motor->PLL.eHz * PI_2;

	const float n = 0.01f;
	motor->PLL.filterd_eHz = motor->PLL.eHz* n + (1-n) * motor->PLL.filterd_eHz;
}

float limit_val(float input, float upper_limit, float lower_limit){
	//Limit an value to the positiv and negatic limits
	return minf(maxf(input, lower_limit),upper_limit);
}

void WESC_Current_Controller(WESC_motor_struct* motor){
	//Computes the Id and Iq current controller. Output Vd and Vq

	//Error calculation
	motor->FOC.error.Id_err = (motor->Input.internal_current_target_d - motor->FOC.Idq.Id) * motor->FOC.pi.Id_p;
	motor->FOC.error.Iq_err = (motor->Input.internal_current_target_q - motor->FOC.Idq.Iq) * motor->FOC.pi.Iq_p;

	//Integral
	motor->FOC.integral.Id_int_err = motor->FOC.integral.Id_int_err + motor->FOC.error.Id_err * motor->FOC.pi.Id_i * motor->hardware.timings.Control_Period;
	motor->FOC.integral.Iq_int_err = motor->FOC.integral.Iq_int_err + motor->FOC.error.Iq_err * motor->FOC.pi.Iq_i * motor->hardware.timings.Control_Period;

	//Limit the integral
	motor->FOC.integral.Id_int_err = limit_val(motor->FOC.integral.Id_int_err, motor->FOC.lim.FOC_Voltage_lim, -motor->FOC.lim.FOC_Voltage_lim);
	motor->FOC.integral.Iq_int_err = limit_val(motor->FOC.integral.Iq_int_err, motor->FOC.lim.FOC_Voltage_lim, -motor->FOC.lim.FOC_Voltage_lim);

	//Calculate the PI controller
	motor->FOC.Vdq.Vd = motor->FOC.error.Id_err + motor->FOC.integral.Id_int_err;
	motor->FOC.Vdq.Vd = limit_val(motor->FOC.Vdq.Vd, motor->FOC.lim.FOC_Vd, -motor->FOC.lim.FOC_Vd);
	motor->FOC.Vdq.Vq = motor->FOC.error.Iq_err + motor->FOC.integral.Iq_int_err;

	//Limit the absolute length of the controlled current vectors
	motor->FOC.lim.FOC_Absolute_Voltage = sqrtf(motor->FOC.Vdq.Vd*motor->FOC.Vdq.Vd + motor->FOC.Vdq.Vq*motor->FOC.Vdq.Vq);
	if(motor->FOC.lim.FOC_Absolute_Voltage > motor->FOC.lim.FOC_Voltage_lim){
		float inv_absolut_length = 1.f/motor->FOC.lim.FOC_Absolute_Voltage;
		motor->FOC.Vdq.Vd = motor->FOC.Vdq.Vd * inv_absolut_length * motor->FOC.lim.FOC_Absolute_Voltage;
		motor->FOC.Vdq.Vq = motor->FOC.Vdq.Vq * inv_absolut_length * motor->FOC.lim.FOC_Absolute_Voltage;
	}
}

void WESC_get_openloop_angle(WESC_motor_struct *motor){
	motor->FOC.FOC_Angle = motor->FOC.FOC_Angle + motor->Observer.openloop.openloop_speed_angle_add;
}

void WESC_set_openloop_rpm(WESC_motor_struct *motor){
	float factor = (Angle_base / (60.f * motor->hardware.timings.Control_Frequency)) * motor->motor_para.Pole_pairs;
	motor->Observer.openloop.openloop_speed_angle_add = (uint16_t)(factor * motor->Observer.openloop.openloop_rpm);

}


void WESC_set_internal_target(WESC_motor_struct *motor){
	switch(motor->control)
	{
	default:
	case CONTROL_TORQUE:
		if(fabsf(motor->Input.external_current_target) < motor->motor_para.max_phase_current){
			motor->Input.internal_current_target_q = motor->Input.external_current_target;
		}
		else if(motor->Input.external_current_target >= motor->motor_para.max_phase_current){
			motor->Input.internal_current_target_q = motor->motor_para.max_phase_current;
		}
		else{
			motor->Input.internal_current_target_q = -motor->motor_para.max_phase_current;
		}
		break;
	case CONTROL_RPM:
		if(fabsf(motor->Input.external_rpm_target) < motor->motor_para.max_rpm){
			motor->Input.internal_rpm_target = motor->Input.external_rpm_target;
		}
		else if(motor->Input.external_current_target >= motor->motor_para.max_rpm){
			motor->Input.internal_rpm_target = motor->motor_para.max_rpm;
		}
		else{
			motor->Input.internal_rpm_target = -motor->motor_para.max_rpm;
		}
		//Convert the RPM target to eHz
		motor->Input.internal_eHz_target = motor->Input.internal_rpm_target * (1.f/60.f) * motor->motor_para.Pole_pairs;
		break;
	case FIXED_VOLTAGE:
		break;
	}
}

void WESC_safety_lock(WESC_motor_struct *motor){
	// Counts down the safety lock Value as long as the throttle input is under the max startup value
	if((abs(motor->Input.external_current_target) > motor->lock.max_allowed_current_input ||abs(motor->Input.external_rpm_target) > motor->lock.max_allowed_rpm_input) && motor->lock.lock_status > 0){
		WESC_enable_safety_lock(motor);
		motor->Input.external_current_target = 0;
		motor->lock.max_allowed_current_input = 0;
	}
	else if(motor->lock.lock_status > 0){
		motor->lock.lock_status--;
	}
}

void WESC_enable_safety_lock(WESC_motor_struct *motor){
	//Enables the Safetylock (ignorance of input)
	motor->lock.lock_status = motor->lock.lock_time;
}

void WESC_calculate_power(WESC_motor_struct *motor){
	motor->FOC.pwr.power_q = 1.5f * motor->FOC.Vdq.Vq * motor->FOC.Idq.Iq_filtered;
	motor->FOC.pwr.power_d = 1.5f * motor->FOC.Vdq.Vd * motor->FOC.Idq.Id_filtered;

	motor->FOC.pwr.power = motor->FOC.pwr.power_d + motor->FOC.pwr.power_q;
}

void WESC__power_controller(WESC_motor_struct *motor){
	//should protect the motor from over power/current conditions
	static uint32_t cycle_counter = 0;
	cycle_counter++;
	WESC_calculate_power(motor);
	//ToDo
}

void WESC_speed_controller(WESC_motor_struct *motor){
	motor->speed_con.error = (motor->Input.internal_eHz_target - motor->PLL.eHz);

	//Calculate integral
	motor->speed_con.integrator = motor->speed_con.integrator + motor->speed_con.error * motor->speed_con.ki * motor->hardware.timings.Slow_loop_Periode;

	//Limit the integral
	motor->speed_con.integrator = limit_val(motor->speed_con.integrator, motor->Input.internal_current_target_q, -motor->Input.internal_current_target_q);

	float current_target = motor->speed_con.error * motor->speed_con.kp + motor->speed_con.integrator;

	motor->Input.internal_current_target_q = limit_val(current_target, motor->motor_para.max_phase_current, -motor->motor_para.max_phase_current);
}

void WESC_filter_iqd(WESC_motor_struct *motor){
	motor->FOC.Idq.Id_filtered = motor->FOC.Idq.filter_factor * motor->FOC.Idq.Id + (1.f-motor->FOC.Idq.filter_factor) * motor->FOC.Idq.Id_filtered;
	motor->FOC.Idq.Iq_filtered = motor->FOC.Idq.filter_factor * motor->FOC.Idq.Iq + (1.f-motor->FOC.Idq.filter_factor) * motor->FOC.Idq.Iq_filtered;
}
