/*
 * WESC_hardware_fct.h
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

#ifndef INC_WESC_HARDWARE_FCT_H_
#define INC_WESC_HARDWARE_FCT_H_

#include "main.h"
#include "WESC_structs.h"

#define Control_Frequency_ 15000.f
#define PWM_Frequency_ Control_Frequency_*2.f
#define Slow_loop_Frequency_ 200.f
#define Deadtime 520e-9 //in seconds

#define max_Voltage 50.f
#define min_Voltage 10.f
#define max_Current 15.f
#define Current_OP_Gain 24.f
#define shunt_resistance 0.005f
#define max_adc_current_measurement 65536 //max value the ADC for the current measurement has, typically 2^n
#define reference_voltage 3.3f// ADC reference voltage

#define Voltage_divider_factor (80000.f+4990.f)/4990.f //Factor to convert the measured voltage on the voltage divider to the actual voltage
#define max_adc_Voltage_measurement 65536 //max value the ADC for the voltage measurement has, typically 2^n


void WESC_hardware_start_cycle_counter(WESC_motor_struct *motor);

uint32_t WESC_hardware_get_cycle_counter(WESC_motor_struct *motor);

void WESC_init_hardware(WESC_motor_struct *motor);

uint8_t WESC_motortimer_downcounting(WESC_motor_struct *motor);

void WESC_sin_cos_fast(uint16_t angle, float *Sin, float *Cos);

void WESC_set_PWM_register(WESC_motor_struct *motor);

void WESC_get_ADC_current_data(WESC_motor_struct *motor);

void WESC_Disable_PWM(WESC_motor_struct *motor);

void WESC_Enable_PWM(WESC_motor_struct *motor);

void WESC_init_hardware_param(WESC_motor_struct *motor);

void WESC_start_absolute_encoder_request(WESC_motor_struct *motor);

void WESC_get_absolute_encoder_value(WESC_motor_struct *motor);

void WESC_get_VBUS_data(WESC_motor_struct *motor);

void WESC_transmit_serial(const uint8_t *pData, uint16_t Size);

void WESC_start_serial(uint8_t *pData, uint16_t Size);

void WESC_setup_serial(void);


//Function from cli_WESC_setup.h
void WESC_cli_init(void);

void WESC_serial_callback(uint8_t size);
//end CLI STUFF

#endif /* INC_WESC_HARDWARE_FCT_H_ */
