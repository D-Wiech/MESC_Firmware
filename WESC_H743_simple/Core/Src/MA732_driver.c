/*
 * MA732_driver.c
 *
 *  Created on: May 31, 2024
 *      Author: Daniel
 */

#include "MA732_driver.h"
#include "main.h"
#include "us_time.h"



void MA732_read(MA732_handler *driver, uint16_t* angle)
{
	HAL_GPIO_WritePin(driver->GPIO_PORT, driver->CS_PIN, GPIO_PIN_RESET);
	uint32_t Timeout = 1;
	uint8_t data_in[2] = {0};
	uint8_t data_out[2] = {0};
	//Sends and Receives two bytes (one massage)
	HAL_StatusTypeDef return_Value = HAL_OK;
	return_Value = HAL_SPI_TransmitReceive(driver->SPI_Handle, data_out, data_in, 1, Timeout);
	if(return_Value !=  HAL_OK){
		//printf("SPI_SENSOR_ERROR\n");
	}
	HAL_GPIO_WritePin(driver->GPIO_PORT, driver->CS_PIN, GPIO_PIN_SET);
	*angle = ((uint16_t)data_in[1])<<8 | data_in[0];
}

float MA732_get_angle(MA732_handler *driver)
{
	const float PI2 =  6.28318531f;
	uint16_t angle =0;
	MA732_read(driver, &angle);
	return ((((float)angle)*PI2)/65536.0f);
}

void MA732_setup(MA732_handler *driver)
{
	HAL_GPIO_WritePin( driver->GPIO_PORT, driver->CS_PIN, GPIO_PIN_SET );
}

void MA732_dma_start_read(MA732_handler *driver){
	HAL_GPIO_WritePin(driver->GPIO_PORT, driver->CS_PIN, GPIO_PIN_RESET);
	HAL_StatusTypeDef return_Value = HAL_OK;
	uint16_t size = 1;
	return_Value = HAL_SPI_Receive_DMA(driver->SPI_Handle, &driver->angle, size);
	if(return_Value !=  HAL_OK){
		//printf("SPI_SENSOR_ERROR\n");
	}
}

void MA732_dma_stop_read(MA732_handler *driver, uint16_t angle){
	HAL_GPIO_WritePin(driver->GPIO_PORT, driver->CS_PIN, GPIO_PIN_SET);
	angle = driver->angle;
}
