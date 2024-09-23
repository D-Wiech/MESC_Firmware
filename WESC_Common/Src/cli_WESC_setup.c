/*
 * cli_WESC_setup.c
 *
 *  Created on: Aug 9, 2024
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

#define CLI_BUFFER_SIZE 128
#define CLI_COMMAND_COUNT 19

#define CLI_ENTRY(command) { command ## Callback, (const CLI_BUF_VALUE_T *) #command }

void cliTxCallback(CLI_BUF_VALUE_T *buf, CLI_TX_BUF_COUNT_VALUE_T bufc);

static CLIRet_t helloworldCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t helpCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t Interrupt_timeCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t OpenloopCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t SetCurrentCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t stopCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t startCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t ShowStatusCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t setfixedVoltageCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t FixedVoltageStartCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t useAbsoluteObserverCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t setAbsoluteOffsetCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t setrpmCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t speedki_setCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t speedkp_setCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t pllkp_setCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t pllki_setCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t resetCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);
static CLIRet_t CURRENT_BW_setCallback(void *args, CLI_ARG_COUNT_VALUE_T argc);


#define RECEIVE_BUFFER_SIZE 128
uint8_t RxBuffer[RECEIVE_BUFFER_SIZE] = {0};


CLIConfig_t l_cli_cnf;
CLIInst_t l_cli_inst;
CLI_BUF_VALUE_T l_cli_buf[CLI_BUFFER_SIZE];
CLICommand_t l_cli_commands[CLI_COMMAND_COUNT] =
{
    CLI_ENTRY(helloworld),
	CLI_ENTRY(help),
	CLI_ENTRY(Interrupt_time),
	CLI_ENTRY(Openloop),
	CLI_ENTRY(SetCurrent),
	CLI_ENTRY(stop),
	CLI_ENTRY(start),
	CLI_ENTRY(ShowStatus),
	CLI_ENTRY(setfixedVoltage),
	CLI_ENTRY(FixedVoltageStart),
	CLI_ENTRY(useAbsoluteObserver),
	CLI_ENTRY(setAbsoluteOffset),
	CLI_ENTRY(setrpm),
	CLI_ENTRY(speedki_set),
	CLI_ENTRY(speedkp_set),
	CLI_ENTRY(pllkp_set),
	CLI_ENTRY(pllki_set),
	CLI_ENTRY(reset),
	CLI_ENTRY(CURRENT_BW_set),
};

static CLIRet_t helloworldCallback(void *args, CLI_ARG_COUNT_VALUE_T argc)
{
    void *buf = NULL;
    CLIArg_t arg = {0U};
    const char msg[] = "Hello World: ";

    cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));

    do
    {
        buf = CLIArgParse(&l_cli_inst, &arg, args, argc);

        if (buf)
        {
            cliTxCallback((CLI_BUF_VALUE_T *) buf, strlen((char *) buf));
        }
    } while (buf != NULL);

    return CLI_OK;
}

static CLIRet_t helpCallback(void *args, CLI_ARG_COUNT_VALUE_T argc)
{
    const char msg[] = "\n\rFollowing commands are available:";
    const char msg1[] = "\n\rCommand ";
    const char msg2[] = ": ";
    char number[30];
    cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
    uint8_t count = l_cli_inst.config.commandc;
    while(count > 0)
    {
    	cliTxCallback((CLI_BUF_VALUE_T *) msg1, strlen(msg1));
    	sprintf(number, "%u", count);
    	cliTxCallback((CLI_BUF_VALUE_T *) number, strlen(number));
    	cliTxCallback((CLI_BUF_VALUE_T *) msg2, strlen(msg2));
    	count--;
    	cliTxCallback((CLI_BUF_VALUE_T *) l_cli_inst.config.commands[count].command, strlen((char*)l_cli_inst.config.commands[count].command));
    }
    return CLI_OK;
}

void cliTxCallback(CLI_BUF_VALUE_T *buf, CLI_TX_BUF_COUNT_VALUE_T bufc)
{
	//Implement an hardware function that sends a serial message via uart or for example usb
	WESC_transmit_serial(buf, bufc);
}

void WESC_cli_init(void){
	  l_cli_cnf.buf = l_cli_buf;
	  l_cli_cnf.bufc = CLI_BUFFER_SIZE;
	  l_cli_cnf.commands = l_cli_commands;
	  l_cli_cnf.commandc = CLI_COMMAND_COUNT;
	  l_cli_cnf.tx = cliTxCallback;

	  CLIInit(&l_cli_inst, l_cli_cnf);

	  WESC_setup_serial();
	  WESC_start_serial(RxBuffer, RECEIVE_BUFFER_SIZE);
}

void WESC_serial_callback(uint8_t size){
	while(size > 0)
	{
		size = size - 1;
		CLIInsert(&l_cli_inst, RxBuffer[size]);
	}
	CLIHandle(&l_cli_inst);
	WESC_start_serial(RxBuffer, RECEIVE_BUFFER_SIZE);
}

static CLIRet_t Interrupt_timeCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	const float MCU_PERIOD = 1.f/300000000.f * 1000000.f;
	char text[128];
	sprintf(text, "PWM_Handler time(us): %f; ADC_Handler time(us): %f; Slow_loop_Handler time(us): %f \n", (float)mtr[0].Stats.PWM_handler_cycles * MCU_PERIOD, (float)mtr[0].Stats.ADC_handler_cycles * MCU_PERIOD, (float)mtr[0].Stats.SlowLoop_handler_cycles * MCU_PERIOD);
	cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
    return CLI_OK;
}


static CLIRet_t OpenloopCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	if (argc != 1) {
		    const char msg[] = "ERROR 1";
		    cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
		    return CLI_OK;
	}
	// Cast von args zu char* (weil es ASCII-Zeichen enthält)
	const char *ascii_str = (const char *)args;

    // Umwandlung des ASCII-Strings zu float
    char *endptr;  // Zeiger für Fehlerprüfung
    float rpm = strtof(ascii_str, &endptr);

	if(fabsf(rpm) < mtr[0].motor_para.max_rpm){
		mtr[0].sensor_type = SENSOR_OPENLOOP;
		mtr[0].Observer.openloop.openloop_rpm = rpm;
		WESC_set_openloop_rpm(&mtr[0]);
		char text[128];
		sprintf(text, "Sensor changed to Openloop with %frpm", rpm);
		cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
		return CLI_OK;
	}
    const char msg[] = "ERROR 2";
    cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
    return CLI_OK;
}

static CLIRet_t SetCurrentCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	if (argc != 1) {
		    const char msg[] = "ERROR 1";
		    cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
		    return CLI_OK;
	}

	// Cast von args zu char* (weil es ASCII-Zeichen enthält)
	const char *ascii_str = (const char *)args;

    // Umwandlung des ASCII-Strings zu float
    char *endptr;  // Zeiger für Fehlerprüfung
    float current = strtof(ascii_str, &endptr);

	if(fabsf(current) < mtr[0].motor_para.max_phase_current){
		mtr[0].control = CONTROL_TORQUE;
		mtr[0].Input.external_current_target = current;
		char text[128];
		sprintf(text, "Current target set to %f", current);
		cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
		return CLI_OK;
	}
  const char msg[] = "ERROR 2";
  cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
  return CLI_OK;
}

static CLIRet_t stopCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	mtr[0].MotorState = MOTOR_TRACKING;
	mtr[0].Output.inverter_output_enable = 0;
	WESC_Disable_PWM(&mtr[0]);

	const char msg[] = "MotorState set to tracking!";
	cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));

	return CLI_OK;
}

static CLIRet_t startCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	mtr[0].MotorState = MOTOR_RUNNING;
	mtr[0].Output.inverter_output_enable = 1;
	WESC_Enable_PWM(&mtr[0]);

	const char msg[] = "MotorState set to running!";
	cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));

	return CLI_OK;
}

static CLIRet_t ShowStatusCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	char text[128];
	sprintf(text, "eHz: %f", mtr[0].PLL.eHz);
	cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
	sprintf(text, "\n\rCurrent target: %fA", mtr[0].Input.external_current_target);
	cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
	sprintf(text, "\n\rRPM target: %frpm", mtr[0].Input.external_rpm_target);
	cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
	sprintf(text, "\n\rSpeed_Ki value: %f", mtr[0].speed_con.ki);
	cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
	sprintf(text, "\n\rSpeed_Kp value: %f", mtr[0].speed_con.kp);
	cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
	sprintf(text, "\n\rPLL_Ki value: %f", mtr[0].PLL.ki);
	cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
	sprintf(text, "\n\rPLL_Kp value: %f", mtr[0].PLL.kp);
	cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
	sprintf(text, "\n\rCurrent_Bandwidth value: %f", mtr[0].motor_para.CURRENT_BW);
	cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
	sprintf(text, "\n\rMotor power: %fW", mtr[0].FOC.pwr.power);
	cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
	return CLI_OK;
}

static CLIRet_t setfixedVoltageCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	if (argc != 1) {
		    const char msg[] = "ERROR 1";
		    cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
		    return CLI_OK;
	}

	// Cast von args zu char* (weil es ASCII-Zeichen enthält)
	const char *ascii_str = (const char *)args;

    // Umwandlung des ASCII-Strings zu float
    char *endptr;  // Zeiger für Fehlerprüfung
    float voltage = strtof(ascii_str, &endptr);

	if(fabsf(voltage) < mtr[0].Con_val.Vbus){
		mtr[0].control = FIXED_VOLTAGE;
		mtr[0].fixedVoltage.Voltage = voltage;
		char text[128];
		sprintf(text, "Fixed Voltage set to %f", voltage);
		cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
		return CLI_OK;
	}
  const char msg[] = "ERROR 2";
  cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
  return CLI_OK;
}

static CLIRet_t FixedVoltageStartCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	mtr[0].MotorState = MOTOR_FIXED_VOLTAGE;
	mtr[0].Output.inverter_output_enable = 1;
	WESC_Enable_PWM(&mtr[0]);

	const char msg[] = "MotorState set to MOTOR_FIXED_VOLTAGE!";
	cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));

	return CLI_OK;
}

static CLIRet_t useAbsoluteObserverCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	mtr[0].sensor_type = SENSOR_ABSOLUTE;
	char text[128];
	sprintf(text, "Sensor changed to Absolute Encoder");
	cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
	return CLI_OK;
}

static CLIRet_t setAbsoluteOffsetCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	if (argc != 1) {
		    const char msg[] = "ERROR 1";
		    cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
		    return CLI_OK;
	}
	// Cast von args zu char* (weil es ASCII-Zeichen enthält)
	const char *ascii_str = (const char *)args;

    // Umwandlung des ASCII-Strings zu float
    char *endptr;  // Zeiger für Fehlerprüfung
    float offset_360 = strtof(ascii_str, &endptr);

	if(fabsf(offset_360) < 360.f * 1.5f){
		float offset_int = offset_360 * 65536.f / 360.f;
		mtr[0].Observer.absolute.angle_offset = offset_int;
		char text[128];
		sprintf(text, "Absolute angle offset set to %f degree", offset_360);
		cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
		return CLI_OK;
	}
    const char msg[] = "ERROR 2";
    cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
    return CLI_OK;
}

static CLIRet_t setrpmCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	if (argc != 1) {
		    const char msg[] = "ERROR 1";
		    cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
		    return CLI_OK;
	}

	// Cast von args zu char* (weil es ASCII-Zeichen enthält)
	const char *ascii_str = (const char *)args;

    // Umwandlung des ASCII-Strings zu float
    char *endptr;  // Zeiger für Fehlerprüfung
    float rpm = strtof(ascii_str, &endptr);

	if(fabsf(rpm) < mtr[0].motor_para.max_rpm){
		mtr[0].control = CONTROL_RPM;
		mtr[0].Input.external_rpm_target = rpm;
		char text[128];
		sprintf(text, "RPM target set to %f rpm", rpm);
		cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
		return CLI_OK;
	}
  const char msg[] = "ERROR 2";
  cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
  return CLI_OK;
}

static CLIRet_t speedki_setCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	if (argc != 1) {
		    const char msg[] = "ERROR 1";
		    cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
		    return CLI_OK;
	}

	// Cast von args zu char* (weil es ASCII-Zeichen enthält)
	const char *ascii_str = (const char *)args;

    // Umwandlung des ASCII-Strings zu float
    char *endptr;  // Zeiger für Fehlerprüfung
    float ki = strtof(ascii_str, &endptr);

	if(fabsf(ki) < 200){
		mtr[0].speed_con.ki = ki;
		char text[128];
		sprintf(text, "Ki set to %f", ki);
		cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
		return CLI_OK;
	}
  const char msg[] = "ERROR 2";
  cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
  return CLI_OK;
}

static CLIRet_t speedkp_setCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	if (argc != 1) {
		    const char msg[] = "ERROR 1";
		    cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
		    return CLI_OK;
	}

	// Cast von args zu char* (weil es ASCII-Zeichen enthält)
	const char *ascii_str = (const char *)args;

    // Umwandlung des ASCII-Strings zu float
    char *endptr;  // Zeiger für Fehlerprüfung
    float kp = strtof(ascii_str, &endptr);

	if(fabsf(kp) < 200){
		mtr[0].speed_con.kp = kp;
		char text[128];
		sprintf(text, "Kp set to %f", kp);
		cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
		return CLI_OK;
	}
  const char msg[] = "ERROR 2";
  cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
  return CLI_OK;
}

static CLIRet_t pllkp_setCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	if (argc != 1) {
		    const char msg[] = "ERROR 1";
		    cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
		    return CLI_OK;
	}

	// Cast von args zu char* (weil es ASCII-Zeichen enthält)
	const char *ascii_str = (const char *)args;

    // Umwandlung des ASCII-Strings zu float
    char *endptr;  // Zeiger für Fehlerprüfung
    float kp = strtof(ascii_str, &endptr);

	if(fabsf(kp) < 200){
		mtr[0].PLL.kp = kp;
		char text[128];
		sprintf(text, "PLL_Kp set to %f", kp);
		cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
		return CLI_OK;
	}
  const char msg[] = "ERROR 2";
  cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
  return CLI_OK;
}

static CLIRet_t pllki_setCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	if (argc != 1) {
		    const char msg[] = "ERROR 1";
		    cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
		    return CLI_OK;
	}

	// Cast von args zu char* (weil es ASCII-Zeichen enthält)
	const char *ascii_str = (const char *)args;

    // Umwandlung des ASCII-Strings zu float
    char *endptr;  // Zeiger für Fehlerprüfung
    float ki = strtof(ascii_str, &endptr);

	if(fabsf(ki) < 200){
		mtr[0].PLL.ki = ki;
		char text[128];
		sprintf(text, "PLL_Ki set to %f", ki);
		cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
		return CLI_OK;
	}
  const char msg[] = "ERROR 2";
  cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
  return CLI_OK;
}

static CLIRet_t resetCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	if (argc != 1) {
		    const char msg[] = "ERROR 1";
		    cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
		    return CLI_OK;
	}
	else{
		NVIC_SystemReset();
	}
}

static CLIRet_t CURRENT_BW_setCallback(void *args, CLI_ARG_COUNT_VALUE_T argc){
	if (argc != 1) {
		    const char msg[] = "ERROR 1";
		    cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
		    return CLI_OK;
	}

	// Cast von args zu char* (weil es ASCII-Zeichen enthält)
	const char *ascii_str = (const char *)args;

    // Umwandlung des ASCII-Strings zu float
    char *endptr;  // Zeiger für Fehlerprüfung
    float BW = strtof(ascii_str, &endptr);

	if(fabsf(BW) < 20000){
		mtr[0].motor_para.CURRENT_BW = BW;
		mtr[0].FOC.pi.Id_p = mtr[0].motor_para.CURRENT_BW * mtr[0].motor_para.Ld;
		mtr[0].FOC.pi.Id_i = mtr[0].motor_para.R / mtr[0].motor_para.Ld;

		mtr[0].FOC.pi.Iq_p = mtr[0].FOC.pi.Id_p ;
		mtr[0].FOC.pi.Iq_i = mtr[0].FOC.pi.Id_i;
		char text[128];
		sprintf(text, "Current bandwidth set to %f", BW);
		cliTxCallback((CLI_BUF_VALUE_T *) text, strlen(text));
		return CLI_OK;
	}
  const char msg[] = "ERROR 2";
  cliTxCallback((CLI_BUF_VALUE_T *) msg, strlen(msg));
  return CLI_OK;
}



