/*
 * infineon_6EDL7141.c
 *
 *  Created on: Mar 31, 2024
 *      Author: Daniel
 */

#include "./infineon_6EDL7141.h"
#include "us_time.h"
#include "spi.h"
#include "gpio.h"

#define BUS_TIMEOUT_6EDL 0xF

int Infineon6EDL7141Driver_readConfig(struct _6EDL7141_driver *driver, struct _6EDL7141_config* config);
int Infineon6EDL7141Driver_readFaultAndWarningStatus(struct _6EDL7141_driver *driver, struct _6EDL7141_stat* status);
int Infineon6EDL7141Driver_writeConfig(struct _6EDL7141_driver *driver, struct _6EDL7141_config* config);
int Infineon6EDL7141Driver_readStatus(struct _6EDL7141_driver *driver, struct _6EDL7141_stat* status);
int Infineon6EDL7141Driver_setCSAMPgain(struct _6EDL7141_driver *driver, struct _6EDL7141_config* config, enum CurrentSenseGain);
int Infineon6EDL7141Driver_setDeadTime(struct _6EDL7141_driver *driver, struct _6EDL7141_config* config, uint8_t Deadtime_in_nanoseconds);

int Infineon6EDL7141Driver_init(struct _6EDL7141_driver *driver, struct _6EDL7141_stat* status, struct _6EDL7141_config* config)
{
	HAL_GPIO_WritePin(driver->GPI_PORT, driver->CS_PIN, GPIO_PIN_SET);
	usdelay(100);
	//Infineon6EDL7141Driver_readFaultAndWarningStatus(driver, status);
	int reslt = Infineon6EDL7141Driver_readStatus(driver, status);
	if(reslt != 0) return 1;
	Infineon6EDL7141Driver_readConfig(driver, config);

	//TODO Set the wanted config. Datasheet: https://www.infineon.com/dgdl/Infineon-6EDL7141-DataSheet-v01_04-EN.pdf?fileId=5546d46279cccfdb0179f4c1d3bf0756
	config->supply_cfg =  dis_cp_pre<<15 | DTOD_200us<<13 | BF_500kHz<<12 | _3V3<<10 | HALF_DVDD<<2| _12V;
	config->adc_cfg = 0;
	config->pwm_cfg = 0;
	config->sensor_cfg = 10<<5;
	config->wd_cfg = 0;
	config->wd_cfg2 = 0;
	config->idrive_cfg = GDC_500mA<<12 | GDC_400mA<<8 | GDC_500mA<<4 | GDC_400mA;
	config->idrive_pre_cfg =  GDC_400mA<<4 | GDC_500mA;
	config->tdrive_src_cfg = 0xFF00;//default
	config->tdrive_sink_cfg = 0xFF00;//default
	config->dt_cfg = 0x3131;//default
	config->cp_cfg = 0x00;//default
	config->csamp_cfg = _8_Events<<14 | DT_0_us<<12 | 0b0<<11 | BT_50_ns<<7 | 0b111<<4 | ViaRegisterConfig<<3 | _32X;
	config->csamp_cfg2 = 0x0833;
	Infineon6EDL7141Driver_writeConfig(driver, config);

	Infineon6EDL7141Driver_readConfig(driver, config);
		// attachInterrupt(digitalPinToInterrupt(nFault), handleInterrupt, PinStatus::FALLING);
	return 0;
}

void Infineon6EDL7141Driver_ENABLE(struct _6EDL7141_driver *driver)
{
	HAL_GPIO_WritePin(driver->NBRAKE_PORT, driver->NBRAKE_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(driver->ENABLE_GPIO_PORT, driver->ENABLE_PIN, GPIO_PIN_SET);
}

int Infineon6EDL7141Driver_readSPI(struct _6EDL7141_driver *driver, uint8_t addr, uint16_t* read_data)
{
	HAL_StatusTypeDef rslt = HAL_OK;
	uint8_t address_size = 1;
	uint8_t data_read[2];
	uint16_t data_size = 2;
	HAL_GPIO_WritePin(driver->GPI_PORT, driver->CS_PIN, GPIO_PIN_RESET);
	usdelay(1);
	rslt = HAL_SPI_Transmit(&driver->SPI_Handle, &addr, address_size, BUS_TIMEOUT_6EDL);
	if(rslt != HAL_OK){
		return 1; // specify the starting register address
	}
	rslt = HAL_SPI_Receive(&driver->SPI_Handle, data_read, data_size, BUS_TIMEOUT_6EDL);
	HAL_GPIO_WritePin(driver->GPI_PORT, driver->CS_PIN, GPIO_PIN_SET);
	*read_data = data_read[0]<<8 | data_read[1];
	if(rslt != HAL_OK){
		return 2;//read the data
	}
	return 0;
}

int Infineon6EDL7141Driver_writeSPI(struct _6EDL7141_driver *driver, uint8_t addr, uint16_t value)
{
	uint8_t write_addr = addr | 0b10000000;
	HAL_StatusTypeDef rslt = HAL_OK;
	uint8_t address_size = 1;
	uint8_t data_write[2];
	data_write[0] = value>>8;
	data_write[1] = value;
	uint16_t data_size = 2;
	HAL_GPIO_WritePin(driver->GPI_PORT, driver->CS_PIN, GPIO_PIN_RESET);
	usdelay(1);
	rslt = HAL_SPI_Transmit(&driver->SPI_Handle, &write_addr, address_size, BUS_TIMEOUT_6EDL);
	if(rslt != HAL_OK){
		return 1; // specify the starting register address
	}
	rslt = HAL_SPI_Transmit(&driver->SPI_Handle, data_write, data_size, BUS_TIMEOUT_6EDL);
	HAL_GPIO_WritePin(driver->GPI_PORT, driver->CS_PIN, GPIO_PIN_SET);
	if(rslt != HAL_OK){
		return 2;//write the data
	}
	return 0;
}

int Infineon6EDL7141Driver_readFaultAndWarningStatus(struct _6EDL7141_driver *driver, struct _6EDL7141_stat* status)
{
	return Infineon6EDL7141Driver_readSPI(driver, FAULT_ST_ADDR, &status->FAULT_STAT);
}

int Infineon6EDL7141Driver_readStatus(struct _6EDL7141_driver *driver, struct _6EDL7141_stat* status)
{
	Infineon6EDL7141Driver_readSPI(driver, DEVICE_ID_ADDR, &status->DEVICE_ID);
	if(status->DEVICE_ID != 0x06) return 1;
	Infineon6EDL7141Driver_readSPI(driver, FAULT_ST_ADDR, &status->FAULT_STAT);
	Infineon6EDL7141Driver_readSPI(driver, TEMP_ST_ADDR, &status->TEMP_STAT);
	Infineon6EDL7141Driver_readSPI(driver, SUPPLY_ST_ADDR, &status->SUPPLY_STAT);
	Infineon6EDL7141Driver_readSPI(driver, FUNCT_ST_ADDR, &status->FUNCT_STAT);
	Infineon6EDL7141Driver_readSPI(driver, OTP_ST_ADDR, &status->OTP_STAT);
	Infineon6EDL7141Driver_readSPI(driver, ADC_ST_ADDR, &status->ADC_STAT);
	Infineon6EDL7141Driver_readSPI(driver, CP_ST_ADDR, &status->CP_STAT);
	return 0;
}



int Infineon6EDL7141Driver_readConfig(struct _6EDL7141_driver *driver, struct _6EDL7141_config* config)
{
	int rslt = 0;
    // Lese jedes Register und speichere den Wert in der Struktur
    Infineon6EDL7141Driver_readSPI(driver, FAULTS_CLR_ADDR, &config->faults_clr);
    Infineon6EDL7141Driver_readSPI(driver, SUPPLY_CFG_ADDR, &config->supply_cfg);
    Infineon6EDL7141Driver_readSPI(driver, ADC_CFG_ADDR, &config->adc_cfg);
    Infineon6EDL7141Driver_readSPI(driver, PWM_CFG_ADDR, &config->pwm_cfg);
    Infineon6EDL7141Driver_readSPI(driver, SENSOR_CFG_ADDR, &config->sensor_cfg);
    Infineon6EDL7141Driver_readSPI(driver, WD_CFG_ADDR, &config->wd_cfg);
    Infineon6EDL7141Driver_readSPI(driver, WD_CFG2_ADDR, &config->wd_cfg2);
    Infineon6EDL7141Driver_readSPI(driver, IDRIVE_CFG_ADDR, &config->idrive_cfg);
    Infineon6EDL7141Driver_readSPI(driver, IDRIVE_PRE_CFG_ADDR, &config->idrive_pre_cfg);
    Infineon6EDL7141Driver_readSPI(driver, TDRIVE_SRC_CFG_ADDR, &config->tdrive_src_cfg);
    Infineon6EDL7141Driver_readSPI(driver, TDRIVE_SINK_CFG_ADDR, &config->tdrive_sink_cfg);
    Infineon6EDL7141Driver_readSPI(driver, DT_CFG_ADDR, &config->dt_cfg);
    Infineon6EDL7141Driver_readSPI(driver, CP_CFG_ADDR, &config->cp_cfg);
    Infineon6EDL7141Driver_readSPI(driver, CSAMP_CFG_ADDR, &config->csamp_cfg);
    Infineon6EDL7141Driver_readSPI(driver, CSAMP_CFG2_ADDR, &config->csamp_cfg2);
    rslt = Infineon6EDL7141Driver_readSPI(driver, OTP_PROG_ADDR, &config->otp_prog);
	return rslt;
}

int Infineon6EDL7141Driver_writeConfig(struct _6EDL7141_driver *driver, struct _6EDL7141_config* config)
{
	int rslt = 0;
    // Schreiben jedes Register und speichere den Wert in der Struktur
	Infineon6EDL7141Driver_writeSPI(driver, FAULTS_CLR_ADDR, config->faults_clr);
	Infineon6EDL7141Driver_writeSPI(driver, SUPPLY_CFG_ADDR, config->supply_cfg);
	Infineon6EDL7141Driver_writeSPI(driver, ADC_CFG_ADDR, config->adc_cfg);
	Infineon6EDL7141Driver_writeSPI(driver, PWM_CFG_ADDR, config->pwm_cfg);
	Infineon6EDL7141Driver_writeSPI(driver, SENSOR_CFG_ADDR, config->sensor_cfg);
	Infineon6EDL7141Driver_writeSPI(driver, WD_CFG_ADDR, config->wd_cfg);
	Infineon6EDL7141Driver_writeSPI(driver, WD_CFG2_ADDR, config->wd_cfg2);
	Infineon6EDL7141Driver_writeSPI(driver, IDRIVE_CFG_ADDR, config->idrive_cfg);
	Infineon6EDL7141Driver_writeSPI(driver, IDRIVE_PRE_CFG_ADDR, config->idrive_pre_cfg);
	Infineon6EDL7141Driver_writeSPI(driver, TDRIVE_SRC_CFG_ADDR, config->tdrive_src_cfg);
	Infineon6EDL7141Driver_writeSPI(driver, TDRIVE_SINK_CFG_ADDR, config->tdrive_sink_cfg);
	Infineon6EDL7141Driver_writeSPI(driver, DT_CFG_ADDR, config->dt_cfg);
	Infineon6EDL7141Driver_writeSPI(driver, CP_CFG_ADDR, config->cp_cfg);
	Infineon6EDL7141Driver_writeSPI(driver, CSAMP_CFG_ADDR, config->csamp_cfg);
    rslt = Infineon6EDL7141Driver_writeSPI(driver, CSAMP_CFG2_ADDR, config->csamp_cfg2);
    //rslt = Infineon6EDL7141Driver_writeSPI(driver, OTP_PROG_ADDR, &config->otp_prog);
	return rslt;
}

int Infineon6EDL7141Driver_setCSAMPgain(struct _6EDL7141_driver *driver, struct _6EDL7141_config* config, enum CurrentSenseGain gain)
{
	int rslt = 0;
	uint16_t csamp_reg_area = 0b111;
	csamp_reg_area = ~csamp_reg_area;
	config->csamp_cfg = (config->csamp_cfg & (csamp_reg_area)) | gain;
	rslt = Infineon6EDL7141Driver_writeSPI(driver, CSAMP_CFG_ADDR, config->csamp_cfg);
	return rslt;
}

int Infineon6EDL7141Driver_setDeadTime(struct _6EDL7141_driver *driver, struct _6EDL7141_config* config, uint8_t Deadtime_in_nanoseconds)
{
	//Deadtime starting by 120ns(b0) in steps of 80ns
	int rslt = 0;
	uint16_t DT_reg_val = ((uint16_t)Deadtime_in_nanoseconds)<<8 | Deadtime_in_nanoseconds;
	config->dt_cfg = DT_reg_val;
	rslt = Infineon6EDL7141Driver_writeSPI(driver, DT_CFG_ADDR, config->dt_cfg);
	return rslt;
}
