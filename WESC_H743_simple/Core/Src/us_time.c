/*
 * us_time.c
 *
 *  Created on: Mar 5, 2024
 *      Author: Daniel
 */

#include "us_time.h"


uint32_t us_elapsed_time(uint32_t* last_timestamp)
{
	uint32_t delta_time = __HAL_TIM_GET_COUNTER(&htim2) - *last_timestamp;
	*last_timestamp = __HAL_TIM_GET_COUNTER(&htim2);
	return delta_time;
}

uint32_t us_get_counter(void)
{
	uint32_t time = __HAL_TIM_GET_COUNTER(&htim2);
	return time;
}

//Starts the timer
void us_time_init(void)
{

	const uint32_t Megahertz = 1000000;
	  //Setting the us-timer registers (ARR and PSC)
	  uint32_t ARR_val = 0xFFFFFFFE;
	  uint16_t PSC_val = 0;
	  PSC_val = ((HAL_RCC_GetHCLKFreq() / Megahertz) - 1);
	  __HAL_TIM_SET_AUTORELOAD(&htim2, ARR_val); //Run PWM-timer at PWM_Frequence
	  __HAL_TIM_SET_PRESCALER(&htim2, PSC_val);



	HAL_TIM_PWM_Start(&htim2,  TIM_CHANNEL_1);
}

void usdelay(uint32_t us_delay)
{
	uint32_t start_of_delay = __HAL_TIM_GET_COUNTER(&htim2);

	while(__HAL_TIM_GET_COUNTER(&htim2) <= (start_of_delay + us_delay));
}
