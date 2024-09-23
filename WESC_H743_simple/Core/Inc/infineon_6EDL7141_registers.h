

#ifndef INFINEON_6EDL7141_REGISTERS
#define INFINEON_6EDL7141_REGISTERS

#include "main.h"
#include <stdint.h>
#include <stdbool.h>
// from https://www.infineon.com/dgdl/Infineon-6EDL7141-DataSheet-v01_08-EN.pdf
// Status Registers - there are 8 status registers, each 16 bits wide
#define FAULT_ST_ADDR 0x00	// Fault and warning status
#define TEMP_ST_ADDR 0x01	// Temperature status
#define SUPPLY_ST_ADDR 0x02 // Power supply status
#define FUNCT_ST_ADDR 0x03	// Functional status
#define OTP_ST_ADDR 0x04	// OTP status
#define ADC_ST_ADDR 0x05	// ADC status
#define CP_ST_ADDR 0x06		// Charge pump status
#define DEVICE_ID_ADDR 0x07 // Device ID

// Control Registers - there are 16 control registers, each 16 bits wide.  Most are R/W
// Note some values only work if set before EN_DRV.  Others only work if written via OTP (see datasheet)
#define FAULTS_CLR_ADDR 0x10	  // Fault clear
#define SUPPLY_CFG_ADDR 0x11	  // Power supply configuration
#define ADC_CFG_ADDR 0x12		  // ADC configuration
#define PWM_CFG_ADDR 0x13		  // PWM configuration
#define SENSOR_CFG_ADDR 0x14	  // Sensor configuration
#define WD_CFG_ADDR 0x15		  // Watchdog configuration
#define WD_CFG2_ADDR 0x16		  // Watchdog configuration 2
#define IDRIVE_CFG_ADDR 0x17	  // Gate driver current configuration
#define IDRIVE_PRE_CFG_ADDR 0x18  // Pre-charge gate driver current configuration
#define TDRIVE_SRC_CFG_ADDR 0x19  // Gate driver sourcing timing configuration
#define TDRIVE_SINK_CFG_ADDR 0x1A // Gate driver sinking timing configuration
#define DT_CFG_ADDR 0x1B		  // Dead time configuration
#define CP_CFG_ADDR 0x1C		  // Charge pump configuration
#define CSAMP_CFG_ADDR 0x1D		  // Current sense amplifier configuration
#define CSAMP_CFG2_ADDR 0x1E	  // Current sense amplifier configuration 2
#define OTP_PROG_ADDR 0x1F		  // OTP program

enum PWMMode
{
	PWM6_Mode = 0b000,
	PWM3_Mode = 0b001,
	PWM1_Mode = 0b010,
	PWM1_Hall_Mode = 0b011
};

enum GateDrivingVoltage
{
	_12V = 0b00,
	_15V = 0b01,
	_10V = 0b10,
	_7V = 0b11
};

enum CurrentSenseReference
{
	HALF_DVDD = 0b00,
	FIVE_TWELTHS_DVDD = 0b01,
	THIRD_DVDD = 0b10,
	QUARTER_DVDD = 0b11
};

enum CurrentThreshold
{
	CT_450mA = 0b00,
	CT_300mA = 0b01,
	CT_150mA = 0b10,
	CT_50mA = 0b11
};

enum DVDDVoltage
{
	ANALOG = 0b00, // the LDO voltage (DVDD) is set using resistors on VSENSE pin
	_3V3 = 0b10,
	_5V = 0b11
};

enum BuckFrequency
{
	BF_500kHz = 0b0,
	BF_1MHz = 0b1
};

enum DVDDTurnOnDelay
{
	DTOD_200us = 0b00,
	DTOD_400us = 0b01,
	DTOD_600us = 0b10,
	DTOD_800us = 0b11
};

enum ADCInputSelection
{
	AIS_IDIGITAL = 0b00,
	AIS_DVDD = 0b01,
	AIS_VDDB = 0b10
};

enum GenericFiltering
{
	GF_8_SAMPLES = 0b00,
	GF_16_SAMPLES = 0b01,
	GF_32_SAMPLES = 0b10,
	GF_64_SAMPLES = 0b11
};

enum PVDDFiltering
{
	PF_32_SAMPLES = 0b00,
	PF_16_SAMPLES = 0b01,
	PF_8_SAMPLES = 0b10,
	PF_1_SAMPLE = 0b11
};

enum OnePWMFreewheel
{
	Active = 0b0, // the low side MOSFETs will be switched synchronously to reduce conduction losses on the body diode conduction
	Diode = 0b1,  // the freewheeling current will flow through the low side MOSFET body diodes.
};

enum BrakeConfig
{
	LowSide = 0b00,
	HighSide = 0b01,
	HighZ = 0b10,
	BrakeToggle = 0b11 // Brake toggle-alternates between low and high side braking on
};

enum CurrentSenseAmplifierTiming
{
	ActiveOnGLxHigh = 0b00,
	ActiveOnGHxLow = 0b01,
	ActiveAlways = 0b10,
};

// Watchdog input selection b000: EN_DRV pin (measure input signal frequency), b001: Reserved, b010: DVDD (linear regulator), b011: VCCLS and VCCHS, (charge pumps), b100: Status register read
// uint16_t WD_FLTCFG : 1;
enum WatchDogInputSelection
{
	EnDrvPin = 0b000,
	DVDD = 0b010,
	ChargePumps = 0b011,
	StatusRegisterRead = 0b100,
};

enum WatchDogFaultConfig
{
	StatusRegisterOnly = 0b00,
	StatusRegisterAndNFaultPin = 0x01,
};

enum DVDDRestartDelay
{
	DRD_500us = 0b0000,
	DRD_1000us = 0b0001,
	DRD_1500us = 0b0010,
	DRD_2000us = 0b0011,
	DRD_2500us = 0b0100,
	DRD_3000us = 0b0101,
	DRD_3500us = 0b0110,
	DRD_4000us = 0b0111,
	DRD_4500us = 0b1000,
	DRD_5000us = 0b1001,
	DRD_5500us = 0b1010,
	DRD_6000us = 0b1011,
	DRD_6500us = 0b1100,
	DRD_7000us = 0b1101,
	DRD_7500us = 0b1110,
	DRD_8000us = 0b1111,
};

enum GateDriverCurrent // for slew control
{
	GDC_10mA = 0b0000,
	GDC_20mA = 0b0001,
	GDC_30mA = 0b0010,
	GDC_40mA = 0b0011,
	GDC_50mA = 0b0100,
	GDC_60mA = 0b0101,
	GDC_80mA = 0b0110,
	GDC_100mA = 0b0111,
	GDC_125mA = 0b1000,
	GDC_150mA = 0b1001,
	GDC_175mA = 0b1010,
	GDC_200mA = 0b1011,
	GDC_250mA = 0b1100,
	GDC_300mA = 0b1101,
	GDC_400mA = 0b1110,
	GDC_500mA = 0b1111,
};

/*
 */
enum ChargePumpClockFrequency
{
	_781_25_kHz = 0b00,
	_390_625_kHz = 0b01,
	_195_3125_kHz = 0b10,
	_1_5625_MHz = 0b11
};

enum ChargePumpPrecharge
{
	en_cp_pre = 0b1,
	dis_cp_pre = 0b0
};

enum CurrentSenseGain
{
	_4X = 0b000,
	_8X = 0b001,
	_12X = 0b010,
	_16X = 0b011,
	_20X = 0b100,
	_24X = 0b101,
	_32X = 0b110,
	_64X = 0b111
};

enum CurrentSenseGainMode
{
	ViaRegisterConfig = 0b0,
	ViaAnalogPinResistr = 0b1
};

enum BlankingTime
{
	BT_0_ns = 0b0000,
	BT_50_ns = 0b0001,
	BT_100_ns = 0b0010,
	BT_200_ns = 0b0011,
	BT_300_ns = 0b0100,
	BT_400_ns = 0b0101,
	BT_500_ns = 0b0110,
	BT_600_ns = 0b0111,
	BT_700_ns = 0b1000,
	BT_800_ns = 0b1001,
	BT_900_ns = 0b1010,
	BT_1_us = 0b1011,
	BT_2_us = 0b1100,
	BT_4_us = 0b1101, // page 93
	BT_6_us = 0b1110,
	BT_8_us = 0b1111,
};

enum DeglitchTime
{
	DT_0_us = 0b00,
	DT_2_us = 0b01,
	DT_4_us = 0b10,
	DT_8_us = 0b11,
};

enum EventCountTrigger
{
	_8_Events = 0b00,
	_16_Events = 0b01,
	All_Events = 0b10,
	Never = 0b11,
};

enum VoltageThreshold
{
	_300_mV = 0b0000,
	_250_mV = 0b0001,
	_225_mV = 0b0010,
	_200_mV = 0b0011,
	_175_mV = 0b0100,
	_150_mV = 0b0101,
	_125_mV = 0b0110,
	_100_mV = 0b0111,
	_90_mV = 0b1000,
	_80_mV = 0b1001,
	_70_mV = 0b1010,
	_60_mV = 0b1011,
	_50_mV = 0b1100,
	_40_mV = 0b1101,
	_30_mV = 0b1110,
	_20_mV = 0b1111,
};

enum CurrentSenseMode
{
	ExternalShunt = 0b0,
	InternalRdson = 0b1, // positive is connected to drain of the low side MOSFET to the positive input of the current sense amplifier
};

enum AutoZero
{
	EnabledWithInternalSynchronization = 0b00,
	Disabled = 0b01,
	EnabledWithExternalSynchronization = 0b10,
	EnabledWithExternalSynchronizationAndClockGating = 0b11,
};

/**
 * Status Registers (READ only)
 */



#endif
