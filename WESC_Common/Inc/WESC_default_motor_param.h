/*
 * WESC_default_motor_param.h
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

#ifndef INC_WESC_DEFAULT_MOTOR_PARAM_H_
#define INC_WESC_DEFAULT_MOTOR_PARAM_H_

#include "main.h"
#include "WESC_structs.h"

void WESC_init_motorparam(WESC_motor_struct *motor);

//Motor defaults
#define Default_Ld 0.000162f //Henry
#define Default_Ld 0.000162f //Henry
#define Default_R 0.365f //Ohm
#define Default_max_phase_Amps 5.f
#define Default_Pole_pairs 4.f
#define Default_Kv 600.f
#define Default_max_Power 75.f
#define Default_max_rpm 4000.f
#define Default_current_BW 5000.f
#define Default_current_filter_factor 0.1f //value between 0 and 1
#define Default_motor_direction 1

#define Default_overpower_duration 10.f //in seconds
#define Default_overpower_break_duration 60.f // Required break time between overpower (in seconds)
#define Default_max_overpower_percentage 1.5f //in decimal
#define Default_max_overcurrent_percentage 1.8f //in decimal

#define Default_safetylock_time 5 //After startup/fault how long the drive should be disabled (time in seconds)
#define Default_safety_current_throttle_percentage 0.05 //max value of the current input that the safety lock disables; in decimal value
#define Default_safety_rpm_throttle_percentage 0.05 ////max value of the rpm input that the safety lock disables; in decimal value

#define Default_speed_ki 0.01f
#define Default_speed_kp 0.01f

#endif /* INC_WESC_DEFAULT_MOTOR_PARAM_H_ */
