/*
 * MESC_Hardware_functions_H743.c
 *
 *  Created on: Aug 4, 2024
 *      Author: Daniel
 */

#include "stm32h7xx_hal.h"
#include "MESCfoc.h"
#include <MESC_Hardware_functions_H743.h>

void Hardware_Debugger_cycle_setup(MESC_motor_typedef *_motor) {
	//enable cycle counter
	DEMCR |= DEMCR_TRCENA;
	DWT_CTRL |= CYCCNTENA;
}
