/*
 * MA732_driver.h
 *
 *  Created on: May 31, 2024
 *      Author: Daniel
 */

#ifndef INC_MA732_DRIVER_H_
#define INC_MA732_DRIVER_H_

#include "main.h"

typedef struct{
	SPI_HandleTypeDef *SPI_Handle;//SPI_handle
	uint16_t CS_PIN;	//CS Pin
	GPIO_TypeDef* GPIO_PORT;//CS Pin port
	uint16_t angle;
}MA732_handler;

void MA732_read(MA732_handler *driver, uint16_t* angle);

void MA732_setup(MA732_handler *driver);

float MA732_get_angle(MA732_handler *driver);

void MA732_dma_start_read(MA732_handler *driver);

void MA732_dma_stop_read(MA732_handler *driver, uint16_t angle);

#endif /* INC_MA732_DRIVER_H_ */
