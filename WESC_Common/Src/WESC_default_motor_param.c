/*
 * WESC_default_motor_param.c
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

 void WESC_init_motorparam(WESC_motor_struct *motor){
	//Load the default motor parameter or parameter from the flash / only default parameter atm
	 motor->motor_para.Kv_const = Default_Kv;
	 motor->motor_para.Ld = Default_Ld;
	 motor->motor_para.Lq = Default_Ld;
	 motor->motor_para.R = Default_R;
	 motor->motor_para.Pole_pairs = Default_Pole_pairs;
	 motor->motor_para.max_phase_current = Default_max_phase_Amps;
	 motor->motor_para.max_power = Default_max_Power;
	 motor->motor_para.max_rpm = Default_max_rpm;

	 //motor overpower/overcurrent handling
	 motor->motor_para.overpower_duration = Default_overpower_duration;
	 motor->motor_para.overpower_break_duration = Default_overpower_break_duration;
	 motor->motor_para.max_overpower_percentage = Default_max_overpower_percentage;
	 motor->motor_para.max_overpower = motor->motor_para.max_power * motor->motor_para.max_overpower_percentage;
	 motor->motor_para.max_overcurrent_percentage = Default_max_overcurrent_percentage;
	 motor->motor_para.max_overcurrent = motor->motor_para.max_phase_current * motor->motor_para.max_overcurrent_percentage;

	 //set the filter factor for the filtered currents Id and Iq;
	 motor->FOC.Idq.filter_factor = Default_current_filter_factor;

	 motor->motor_para.CURRENT_BW = Default_current_BW;

	 //PID controller gains
	 motor->FOC.pi.Id_p = motor->motor_para.CURRENT_BW * motor->motor_para.Ld;
	 motor->FOC.pi.Id_i = motor->motor_para.R / motor->motor_para.Ld;

	 motor->FOC.pi.Iq_p = motor->FOC.pi.Id_p ;
	 motor->FOC.pi.Iq_i = motor->FOC.pi.Id_i;

	 motor->motor_para.motor_direction_factor = Default_motor_direction;

	 motor->lock.lock_time = (uint32_t)motor->hardware.timings.Slow_loop_Frequency * Default_safetylock_time;
	 motor->lock.lock_status = motor->lock.lock_time;
	 motor->lock.max_allowed_current_input = motor->motor_para.max_phase_current * Default_safety_current_throttle_percentage;
	 motor->lock.max_allowed_rpm_input = motor->motor_para.max_rpm * Default_safety_rpm_throttle_percentage;

	 motor->speed_con.integrator = 0;
	 motor->speed_con.ki = Default_speed_ki;
	 motor->speed_con.kp = Default_speed_kp;
}
