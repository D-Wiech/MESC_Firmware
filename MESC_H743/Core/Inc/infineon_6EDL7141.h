

#ifndef INFINEON_6EDL7141
#define INFINEON_6EDL7141

#include "main.h"
#include <SPI.h>
#include "./infineon_6EDL7141_registers.h"

#define _6EDL7141_SPI_Handle hspi4
#define _6EDL7141_SPI_CS

struct _6EDL7141_driver
{
	SPI_HandleTypeDef SPI_Handle;//SPI_handle
	uint16_t CS_PIN;	//CS Pin
	GPIO_TypeDef* GPI_PORT;//CS Pin port
	uint16_t ENABLE_PIN;	//Enable Pin
	GPIO_TypeDef* ENABLE_GPIO_PORT;//CS Pin port
	uint16_t NBRAKE_PIN;	//Enable Pin
	GPIO_TypeDef* NBRAKE_PORT;//CS Pin port
};

struct _6EDL7141_stat
{
	uint16_t FAULT_STAT;
	uint16_t TEMP_STAT;
	uint16_t SUPPLY_STAT;
	uint16_t FUNCT_STAT;
	uint16_t OTP_STAT;
	uint16_t ADC_STAT;
	uint16_t CP_STAT;
	uint16_t DEVICE_ID;
};

struct _6EDL7141_config
{
    uint16_t faults_clr;
    uint16_t supply_cfg;
    uint16_t adc_cfg;
    uint16_t pwm_cfg;
    uint16_t sensor_cfg;
    uint16_t wd_cfg;
    uint16_t wd_cfg2;
    uint16_t idrive_cfg;
    uint16_t idrive_pre_cfg;
    uint16_t tdrive_src_cfg;
    uint16_t tdrive_sink_cfg;
    uint16_t dt_cfg;
    uint16_t cp_cfg;
    uint16_t csamp_cfg;
    uint16_t csamp_cfg2;
    uint16_t otp_prog;
};

int Infineon6EDL7141Driver_init(struct _6EDL7141_driver *driver, struct _6EDL7141_stat* status, struct _6EDL7141_config* config);

int Infineon6EDL7141Driver_readSPI(struct _6EDL7141_driver *driver, uint8_t addr, uint16_t* read_data);

void Infineon6EDL7141Driver_ENABLE(struct _6EDL7141_driver *driver);

int Infineon6EDL7141Driver_readStatus(struct _6EDL7141_driver *driver, struct _6EDL7141_stat* status);

int Infineon6EDL7141Driver_setCSAMPgain(struct _6EDL7141_driver *driver, struct _6EDL7141_config* config, enum CurrentSenseGain);

int Infineon6EDL7141Driver_setDeadTime(struct _6EDL7141_driver *driver, struct _6EDL7141_config* config, uint8_t Deadtime_in_nanoseconds);

#endif
