/*
 * WESC_hardware_fct.c
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
#include "./infineon_6EDL7141.h"
#include "MA732_driver.h"

//Debug
#define DEMCR_TRCENA    0x01000000
#define DEMCR           (*((volatile uint32_t *)0xE000EDFC))
#define DWT_CTRL        (*(volatile uint32_t *)0xe0001000)
#define CYCCNTENA       (1<<0)
#define DWT_CYCCNT      ((volatile uint32_t *)0xE0001004)
#define CPU_CYCLES      *DWT_CYCCNT

void WESC_Disable_PWM(WESC_motor_struct *motor);


// Vector 256 long of sin wave, stretched by 65 to allow computation of cosine as sin(angle+64) without needing wrapping
const float sinwave[321] = { 0.0000000000,  0.0245412285,  0.0490676743,  0.0735645636,  0.0980171403,  0.1224106752,  0.1467304745,  0.1709618888,
							 0.1950903220,  0.2191012402,  0.2429801799,  0.2667127575,  0.2902846773,  0.3136817404,  0.3368898534,  0.3598950365,
							 0.3826834324,  0.4052413140,  0.4275550934,  0.4496113297,  0.4713967368,  0.4928981922,  0.5141027442,  0.5349976199,
							 0.5555702330,  0.5758081914,  0.5956993045,  0.6152315906,  0.6343932842,  0.6531728430,  0.6715589548,  0.6895405447,
							 0.7071067812,  0.7242470830,  0.7409511254,  0.7572088465,  0.7730104534,  0.7883464276,  0.8032075315,  0.8175848132,
							 0.8314696123,  0.8448535652,  0.8577286100,  0.8700869911,  0.8819212643,  0.8932243012,  0.9039892931,  0.9142097557,
							 0.9238795325,  0.9329927988,  0.9415440652,  0.9495281806,  0.9569403357,  0.9637760658,  0.9700312532,  0.9757021300,
							 0.9807852804,  0.9852776424,  0.9891765100,  0.9924795346,  0.9951847267,  0.9972904567,  0.9987954562,  0.9996988187,
							 1.0000000000,  0.9996988187,  0.9987954562,  0.9972904567,  0.9951847267,  0.9924795346,  0.9891765100,  0.9852776424,
							 0.9807852804,  0.9757021300,  0.9700312532,  0.9637760658,  0.9569403357,  0.9495281806,  0.9415440652,  0.9329927988,
							 0.9238795325,  0.9142097557,  0.9039892931,  0.8932243012,  0.8819212643,  0.8700869911,  0.8577286100,  0.8448535652,
							 0.8314696123,  0.8175848132,  0.8032075315,  0.7883464276,  0.7730104534,  0.7572088465,  0.7409511254,  0.7242470830,
							 0.7071067812,  0.6895405447,  0.6715589548,  0.6531728430,  0.6343932842,  0.6152315906,  0.5956993045,  0.5758081914,
							 0.5555702330,  0.5349976199,  0.5141027442,  0.4928981922,  0.4713967368,  0.4496113297,  0.4275550934,  0.4052413140,
							 0.3826834324,  0.3598950365,  0.3368898534,  0.3136817404,  0.2902846773,  0.2667127575,  0.2429801799,  0.2191012402,
							 0.1950903220,  0.1709618888,  0.1467304745,  0.1224106752,  0.0980171403,  0.0735645636,  0.0490676743,  0.0245412285,
							 0.0000000000, -0.0245412285, -0.0490676743, -0.0735645636, -0.0980171403, -0.1224106752, -0.1467304745, -0.1709618888,
							-0.1950903220, -0.2191012402, -0.2429801799, -0.2667127575, -0.2902846773, -0.3136817404, -0.3368898534, -0.3598950365,
							-0.3826834324, -0.4052413140, -0.4275550934, -0.4496113297, -0.4713967368, -0.4928981922, -0.5141027442, -0.5349976199,
							-0.5555702330, -0.5758081914, -0.5956993045, -0.6152315906, -0.6343932842, -0.6531728430, -0.6715589548, -0.6895405447,
							-0.7071067812, -0.7242470830, -0.7409511254, -0.7572088465, -0.7730104534, -0.7883464276, -0.8032075315, -0.8175848132,
							-0.8314696123, -0.8448535652, -0.8577286100, -0.8700869911, -0.8819212643, -0.8932243012, -0.9039892931, -0.9142097557,
							-0.9238795325, -0.9329927988, -0.9415440652, -0.9495281806, -0.9569403357, -0.9637760658, -0.9700312532, -0.9757021300,
							-0.9807852804, -0.9852776424, -0.9891765100, -0.9924795346, -0.9951847267, -0.9972904567, -0.9987954562, -0.9996988187,
							-1.0000000000, -0.9996988187, -0.9987954562, -0.9972904567, -0.9951847267, -0.9924795346, -0.9891765100, -0.9852776424,
							-0.9807852804, -0.9757021300, -0.9700312532, -0.9637760658, -0.9569403357, -0.9495281806, -0.9415440652, -0.9329927988,
							-0.9238795325, -0.9142097557, -0.9039892931, -0.8932243012, -0.8819212643, -0.8700869911, -0.8577286100, -0.8448535652,
							-0.8314696123, -0.8175848132, -0.8032075315, -0.7883464276, -0.7730104534, -0.7572088465, -0.7409511254, -0.7242470830,
							-0.7071067812, -0.6895405447, -0.6715589548, -0.6531728430, -0.6343932842, -0.6152315906, -0.5956993045, -0.5758081914,
							-0.5555702330, -0.5349976199, -0.5141027442, -0.4928981922, -0.4713967368, -0.4496113297, -0.4275550934, -0.4052413140,
							-0.3826834324, -0.3598950365, -0.3368898534, -0.3136817404, -0.2902846773, -0.2667127575, -0.2429801799, -0.2191012402,
							-0.1950903220, -0.1709618888, -0.1467304745, -0.1224106752, -0.0980171403, -0.0735645636, -0.0490676743, -0.0245412285,
							 0.0000000000,  0.0245412285,  0.0490676743,  0.0735645636,  0.0980171403,  0.1224106752,  0.1467304745,  0.1709618888,
							 0.1950903220,  0.2191012402,  0.2429801799,  0.2667127575,  0.2902846773,  0.3136817404,  0.3368898534,  0.3598950365,
							 0.3826834324,  0.4052413140,  0.4275550934,  0.4496113297,  0.4713967368,  0.4928981922,  0.5141027442,  0.5349976199,
							 0.5555702330,  0.5758081914,  0.5956993045,  0.6152315906,  0.6343932842,  0.6531728430,  0.6715589548,  0.6895405447,
							 0.7071067812,  0.7242470830,  0.7409511254,  0.7572088465,  0.7730104534,  0.7883464276,  0.8032075315,  0.8175848132,
							 0.8314696123,  0.8448535652,  0.8577286100,  0.8700869911,  0.8819212643,  0.8932243012,  0.9039892931,  0.9142097557,
							 0.9238795325,  0.9329927988,  0.9415440652,  0.9495281806,  0.9569403357,  0.9637760658,  0.9700312532,  0.9757021300,
							 0.9807852804,  0.9852776424,  0.9891765100,  0.9924795346,  0.9951847267,  0.9972904567,  0.9987954562,  0.9996988187,
							 1.0000000000};
//Angle is defined 0(0) to 2^16(2pi)

void WESC_hardware_start_cycle_counter(WESC_motor_struct *motor){
	//enable cycle counter
	DEMCR |= DEMCR_TRCENA;
	DWT_CTRL |= CYCCNTENA;
}

uint32_t WESC_hardware_get_cycle_counter(WESC_motor_struct *motor){
	return CPU_CYCLES;
}

void WESC_init_hardware_param(WESC_motor_struct *motor){
	motor->hardware.timings.Control_Frequency = Control_Frequency_;
	motor->hardware.timings.Control_Period = 1.f / Control_Frequency_;
	motor->hardware.timings.PWM_Frequency = PWM_Frequency_;
	motor->hardware.timings.PWM_Periode = 1.f / PWM_Frequency_;
	motor->hardware.timings.Slow_loop_Frequency = Slow_loop_Frequency_;
	motor->hardware.timings.Slow_loop_Periode = 1.f / Slow_loop_Frequency_;

	motor->hardware.Current_ADC.Conversion_factor = 1.f/(shunt_resistance * Current_OP_Gain * (float)(max_adc_current_measurement/2) * reference_voltage);
	motor->hardware.Current_ADC.Over_current_limit = max_Current / motor->hardware.Current_ADC.Conversion_factor;
	motor->hardware.Current_ADC.MAX_ADC_val = max_adc_current_measurement;

	motor->hardware.Voltage_ADC.MAX_ADC_val = max_adc_Voltage_measurement;
	motor->hardware.Voltage_ADC.Conversion_factor = (Voltage_divider_factor * reference_voltage)/ (float)max_adc_Voltage_measurement;

	motor->hardware.Voltage_ADC.max_voltage = (max_Voltage * max_adc_Voltage_measurement) / (Voltage_divider_factor * reference_voltage);
	motor->hardware.Voltage_ADC.min_voltage = (min_Voltage * max_adc_Voltage_measurement) / (Voltage_divider_factor * reference_voltage);

}

void WESC_init_hardware(WESC_motor_struct *motor){
	//Configure the Hardware / Start the PWM Handler as late as possible
	  struct _6EDL7141_driver driver_hw;
	  struct _6EDL7141_stat driver_status;
	  struct _6EDL7141_config driver_config;
	  driver_hw.CS_PIN = SPI4_CS_Pin;
	  driver_hw.GPI_PORT = SPI4_CS_GPIO_Port;
	  driver_hw.SPI_Handle = hspi4;
	  driver_hw.ENABLE_PIN = DRIVER_ENABLE_Pin;
	  driver_hw.ENABLE_GPIO_PORT = DRIVER_ENABLE_GPIO_Port;
	  driver_hw.NBRAKE_PIN = DRIVER_NBRAKE_Pin;
	  driver_hw.NBRAKE_PORT = DRIVER_NBRAKE_GPIO_Port;
	  int reslt = Infineon6EDL7141Driver_init(&driver_hw, &driver_status, &driver_config);
	  Infineon6EDL7141Driver_setCSAMPgain(&driver_hw, &driver_config, _24X);
	  Infineon6EDL7141Driver_setDeadTime(&driver_hw, &driver_config, 5);
	  if(reslt == 0) Infineon6EDL7141Driver_ENABLE(&driver_hw);

	  //Configure the absolute encoder
	  motor->Observer.absolute.abs_encoder.CS_PIN = SPI6_CS_Pin;
	  motor->Observer.absolute.abs_encoder.GPIO_PORT = GPIOD;
	  motor->Observer.absolute.abs_encoder.SPI_Handle = &hspi6;
	  MA732_setup(&motor->Observer.absolute.abs_encoder);
	  motor->Observer.absolute.angle_direction = 1;
	  motor->Observer.absolute.angle_offset = (65536*324)/360;//Discovered by testing with fixed Voltage mode and the lowest current wit different angles

	  //Setting the Slowtimer registers (ARR and PSC)
	  uint32_t ARR_val = 99999; //16bit wert
	  uint16_t PSC_val = 0;
	  uint32_t MAX_ARR_VALUE = 65000;
	  ARR_val = ((HAL_RCC_GetHCLKFreq() / (motor->hardware.timings.Slow_loop_Frequency * (PSC_val + 1))) - 1);
	  while (ARR_val > MAX_ARR_VALUE) {
			PSC_val++;
			ARR_val = ((HAL_RCC_GetHCLKFreq() / (motor->hardware.timings.Slow_loop_Frequency * (PSC_val + 1)))	- 1);
	  }

	  __HAL_TIM_SET_AUTORELOAD(motor->stimer, ARR_val); //Run Slowtimer at Slow_Frequence
	  __HAL_TIM_SET_PRESCALER(motor->stimer, PSC_val);

	  //Setting the Motortimer registers (ARR and PSC)
	  ARR_val = 99999;
	  PSC_val = 0;
	  ARR_val = ((HAL_RCC_GetHCLKFreq() / (motor->hardware.timings.PWM_Frequency * (PSC_val + 1))) - 1);
	  while (ARR_val > MAX_ARR_VALUE) {
			PSC_val++;
			ARR_val = ((HAL_RCC_GetHCLKFreq() / (motor->hardware.timings.PWM_Frequency  * (PSC_val + 1)))	- 1);
	  }
	  __HAL_TIM_SET_AUTORELOAD(motor->mtimer, ARR_val); //Run PWM-timer at PWM_Frequence
	  __HAL_TIM_SET_PRESCALER(motor->mtimer, PSC_val);

	  //Calculate the deadtime offset
	  motor->hardware.dead_time.Deadtime_offset = (uint16_t) (Deadtime / ( (motor->hardware.timings.PWM_Periode * 2.f) / (float)motor->mtimer->Instance->ARR));
	  motor->hardware.dead_time.pwm_factor = ((float)(motor->mtimer->Instance->ARR - motor->hardware.dead_time.Deadtime_offset));

	  //Stop all ADCs
	  HAL_ADC_Stop(&hadc1);
	  HAL_ADC_Stop(&hadc2);
	  HAL_ADC_Stop(&hadc3);

	  //Start a calibration for every adc
	  HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);
	  HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);
	  HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);

	  motor->mtimer->Instance->CCR4 = motor->mtimer->Instance->ARR-5; //Just short of dead center (dead center will not actually trigger the conversion)

	  HAL_TIM_PWM_Start(motor->mtimer, TIM_CHANNEL_4 );

	  HAL_TIM_PWM_Start(    motor->mtimer, TIM_CHANNEL_1 );
	  HAL_TIMEx_PWMN_Start( motor->mtimer, TIM_CHANNEL_1 );
	  HAL_TIM_PWM_Start(    motor->mtimer, TIM_CHANNEL_2 );
	  HAL_TIMEx_PWMN_Start( motor->mtimer, TIM_CHANNEL_2 );
	  HAL_TIM_PWM_Start(    motor->mtimer, TIM_CHANNEL_3 );
	  HAL_TIMEx_PWMN_Start( motor->mtimer, TIM_CHANNEL_3 );

	  WESC_Disable_PWM(motor);

	  HAL_ADCEx_InjectedStart( &hadc1 );
	  HAL_ADCEx_InjectedStart( &hadc2 );
	  HAL_ADCEx_InjectedStart( &hadc3 );

	  //Starting the Slowtimer
	  HAL_TIM_PWM_Start(motor->stimer, TIM_CHANNEL_1);
	  __HAL_TIM_ENABLE_IT(motor->stimer, TIM_IT_UPDATE);

	  //Enabling the Motor PWM interrupts
	  __HAL_ADC_ENABLE_IT(&hadc2, ADC_IT_JEOS);
	  __HAL_TIM_ENABLE_IT(motor->mtimer, TIM_IT_UPDATE);
}

uint8_t WESC_motortimer_downcounting(WESC_motor_struct *motor){
	return (motor->mtimer->Instance->CR1 & 0x16);
}

void WESC_sin_cos_fast(uint16_t angle, float *Sin, float *Cos){
	*Sin = sinwave[angle >> 8];
	*Cos = sinwave[(angle >> 8) + 64];
}

void WESC_set_PWM_register(WESC_motor_struct *motor){
	if(motor->Output.inverter_output_enable == 1)
	{
		motor->mtimer->Instance->CCR1 = (uint16_t)(motor->hardware.dead_time.pwm_factor * motor->Output.PWM_U_val) + motor->hardware.dead_time.Deadtime_offset;
		motor->mtimer->Instance->CCR2 = (uint16_t)(motor->hardware.dead_time.pwm_factor * motor->Output.PWM_V_val) + motor->hardware.dead_time.Deadtime_offset;
		motor->mtimer->Instance->CCR3 = (uint16_t)(motor->hardware.dead_time.pwm_factor * motor->Output.PWM_W_val) + motor->hardware.dead_time.Deadtime_offset;
	}
}

void WESC_get_ADC_current_data(WESC_motor_struct *motor){
	//Get the latest current data
	motor->hardware.Current_ADC.Phase_U = hadc3.Instance->JDR1;
	motor->hardware.Current_ADC.Phase_V = hadc2.Instance->JDR1;
	motor->hardware.Current_ADC.Phase_W = hadc1.Instance->JDR1;
}

void WESC_Disable_PWM(WESC_motor_struct *motor)
{
	//Stop the PWM output generation
	HAL_TIM_PWM_Stop(motor->mtimer, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(motor->mtimer, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(motor->mtimer, TIM_CHANNEL_3);
	/*
	uint32_t tmpccmrx;
	//Disable Phase U
	tmpccmrx = motor->mtimer->Instance->CCMR1;
	tmpccmrx &= ~TIM_CCMR1_OC1M;
	tmpccmrx &= ~TIM_CCMR1_CC1S;
	tmpccmrx |= TIM_OCMODE_FORCED_INACTIVE;
	motor->mtimer->Instance->CCMR1 = tmpccmrx;
	motor->mtimer->Instance->CCER &= ~TIM_CCER_CC1E;   // disable
	motor->mtimer->Instance->CCER &= ~TIM_CCER_CC1NE;  // disable
	//Disable Phase V
	tmpccmrx = motor->mtimer->Instance->CCMR1;
	tmpccmrx &= ~TIM_CCMR1_OC2M;
	tmpccmrx &= ~TIM_CCMR1_CC2S;
	tmpccmrx |= TIM_OCMODE_FORCED_INACTIVE << 8;
	motor->mtimer->Instance->CCMR1 = tmpccmrx;
	motor->mtimer->Instance->CCER &= ~TIM_CCER_CC2E;   // disable
	motor->mtimer->Instance->CCER &= ~TIM_CCER_CC2NE;  // disable
	//Disable Phase W
	tmpccmrx = motor->mtimer->Instance->CCMR2;
	tmpccmrx &= ~TIM_CCMR2_OC3M;
	tmpccmrx &= ~TIM_CCMR2_CC3S;
	tmpccmrx |= TIM_OCMODE_FORCED_INACTIVE;
	motor->mtimer->Instance->CCMR2 = tmpccmrx;
	motor->mtimer->Instance->CCER &= ~TIM_CCER_CC3E;   // disable
	motor->mtimer->Instance->CCER &= ~TIM_CCER_CC3NE;  // disable
	*/
}

void WESC_Enable_PWM(WESC_motor_struct *motor){
	//Start the PWM output generation
	HAL_TIM_PWM_Start(motor->mtimer, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(motor->mtimer, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(motor->mtimer, TIM_CHANNEL_3);
	/*
	uint32_t tmpccmrx;
	//Enable Phase U
	tmpccmrx = motor->mtimer->Instance->CCMR1;
	tmpccmrx &= ~TIM_CCMR1_OC1M;
	tmpccmrx &= ~TIM_CCMR1_CC1S;
	tmpccmrx |= TIM_OCMODE_PWM1;
	motor->mtimer->Instance->CCMR1 = tmpccmrx;
	motor->mtimer->Instance->CCER |= TIM_CCER_CC1E;   // enable
	motor->mtimer->Instance->CCER |= TIM_CCER_CC1NE;  // enable
	//Enable Phase V
	tmpccmrx = motor->mtimer->Instance->CCMR1;
	tmpccmrx &= ~TIM_CCMR1_OC2M;
	tmpccmrx &= ~TIM_CCMR1_CC2S;
	tmpccmrx |= TIM_OCMODE_PWM1 << 8;
	motor->mtimer->Instance->CCMR1 = tmpccmrx;
	motor->mtimer->Instance->CCER |= TIM_CCER_CC2E;   // enable
	motor->mtimer->Instance->CCER |= TIM_CCER_CC2NE;  // enable
	//Enable Phase W
	tmpccmrx = motor->mtimer->Instance->CCMR2;
	tmpccmrx &= ~TIM_CCMR2_OC3M;
	tmpccmrx &= ~TIM_CCMR2_CC3S;
	tmpccmrx |= TIM_OCMODE_PWM1;
	motor->mtimer->Instance->CCMR2 = tmpccmrx;
	motor->mtimer->Instance->CCER |= TIM_CCER_CC3E;   // enable
	motor->mtimer->Instance->CCER |= TIM_CCER_CC3NE;  // enable
	*/
}

void WESC_start_absolute_encoder_request(WESC_motor_struct *motor){
	if(motor->sensor_type == SENSOR_ABSOLUTE)
	{
		//MA732_dma_start_read(&motor->Observer.absolute.abs_encoder);
	}
}

void WESC_get_absolute_encoder_value(WESC_motor_struct *motor){
	uint16_t angle = 0;
	//MA732_dma_stop_read(&motor->Observer.absolute.abs_encoder, angle);
	MA732_read(&motor->Observer.absolute.abs_encoder, &angle);
	uint32_t fullangle = 0xFFFF;										//Weird sensor setup require /2 normally not needed
	uint32_t temp_angle = ((uint32_t)angle * (uint32_t)motor->motor_para.Pole_pairs /2 + motor->Observer.absolute.angle_offset)%fullangle;
	if(motor->Observer.absolute.angle_direction == 1){
		temp_angle = fullangle - temp_angle;
	}
	motor->Observer.absolute.absolute_angle = (uint16_t)temp_angle ;
}

void WESC_get_VBUS_data(WESC_motor_struct *motor){
	//Get the latest VBUS data
	motor->hardware.Voltage_ADC.Vbus = hadc3.Instance->JDR2; //Supply voltage
}

void WESC_transmit_serial(const uint8_t *pData, uint16_t Size){
	HAL_UART_Transmit(&huart5, pData, Size, 1);
}

void WESC_start_serial(uint8_t *pData, uint16_t Size){
	HAL_UARTEx_ReceiveToIdle_DMA(&huart5, pData, Size);
}

void WESC_setup_serial(void){
	HAL_NVIC_EnableIRQ(UART5_IRQn);
	HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
	HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
}

