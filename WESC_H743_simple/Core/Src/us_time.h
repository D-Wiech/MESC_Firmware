/*
 * us_time.h
 *
 *  Created on: Mar 5, 2024
 *      Author: Daniel
 */

#ifndef INC_US_TIME_H_
#define INC_US_TIME_H_

//This lib uses TIM2 to implement a "Clock" with microsecond precision

#include "main.h"
#include "tim.h"//only needed when periphieral inits are extern

uint32_t us_elapsed_time(uint32_t* last_timestamp);

void us_time_init(void);

uint32_t us_get_counter(void);

void usdelay(uint32_t us_delay);

#endif /* INC_US_TIME_H_ */
