/*
 * adc_custom.h
 *
 *  Created on: Oct 23, 2020
 *      Author: sznandor
 */

#ifndef INC_CHANNELS_H_
#define INC_CHANNELS_H_

#include "adc.h"
#include "display.h"
#include "tim.h"
#include "gpio.h"
#include "dcmi.h"

#define CHANNELS_NUM				2

#define CH1_ID						0
#define CH2_ID						1

#define CHANNEL_ACTIVE 				0
#define CHANNEL_INACTIVE			1

#define CHANNEL_CHOSEN				0
#define CHANNEL_NOT_CHOSEN			1

#define CHANNEL_TRIGGER_RISING		0
#define CHANNEL_TRIGGER_FALLING 	1

#define CHANNEL_DC					0
#define CHANNEL_AC					1

#define FALSE						0
#define TRUE 						1

#define ADC_BUF_LEN 1024


#define CHANNEL_DIV_1			1
#define CHANNEL_DIV_2			2
#define CHANNEL_DIV_5			5
#define CHANNEL_DIV_10 			10
#define CHANNEL_DIV_20 			20
#define CHANNEL_DIV_50			50
#define CHANNEL_GND				0

#define TRIGGER_SOURCE_CH1		0
#define TRIGGER_SOURCE_CH2		1
#define TRIGGER_SOURCE_EXTADC	2

#define ADC_MCU					0
#define ADC_EXT					1

#define DISPLAYMODE_TIME		0
#define DISPLAYMODE_FFT			1

#define CH1_COLOR				0xF800
#define CH2_COLOR				0x001F


typedef struct channelStruct {
	uint8_t id;
	uint8_t state;
	uint8_t chosen;
	uint8_t div_idx;
	uint8_t div_changed;
	uint8_t sampling_idx;
	uint8_t sampling_changed;
	uint16_t trig_lvl;
	uint8_t trig_edge;
	uint8_t converting;
	uint8_t triggered;
	uint8_t coupling;
	uint8_t coupling_changed;
	uint8_t buff[ADC_BUF_LEN];
	uint16_t buffSize;
	uint16_t buffStart;
	uint8_t bufferFull;
	uint8_t displayMode;
	uint16_t color;
	uint8_t adc;
	float input_voltage_range;
	float frequency;
} channelStruct;

extern uint32_t ADC_SAMPLING_TIMES[];
extern float ADC_timebase_lookup[];
extern float ExtADC_timebase_lookup[];

extern channelStruct channels[];

extern uint8_t ok;

extern uint8_t CH_DIVs[];

extern uint32_t last_dcmi_time;
extern uint32_t last_freq_time;

void setTriggerLevel(float value);
void CH1_setDiv(uint8_t div);
void CH2_setDiv(uint8_t div);
void setTriggerSource(uint8_t source);
void CH1_setCoupling(uint8_t mode);
void CH2_setCoupling(uint8_t mode);
void CH1_selectADC(uint8_t adc);

void disableFrequencyMeasurement();
void enableFrequencyMeasurement();

void findTrigger(uint8_t channel_id);

void updateChannels();

void externalADCompleteCallback();
void resetDCMI();
void disableExtADC();
void enableExtADC();

void startConversion();

#endif /* INC_CHANNELS_H_ */
