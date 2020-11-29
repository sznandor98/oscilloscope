/*
 * io.h
 *
 *  Created on: Sep 27, 2020
 *      Author: sznandor
 */

#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_

#include "channels.h"
#include "main.h"
#include "gpio.h"
#include "display.h"

#define BTN_STATE_PRESSED		1
#define BTN_STATE_RELEASED		0
#define BTN_NUM					5
#define DEBOUNCE_TIME_MS		80

#define ENCODER_NOT_SCROLLED	0
#define ENCODER_SCROLLED_DOWN	1
#define ENCODER_SCROLLED_UP		2

#define PRESSED		1
#define PROCESSED	0

// states
#define STATE_DIV				0
#define STATE_TIMEBASE			1
#define STATE_TRIGGER			2
#define STATE_CHANNELS			4

typedef struct btnStruct {
	uint16_t pin;
	GPIO_TypeDef* port;
	uint8_t state;
	uint8_t isPressed;
	uint32_t pressTime;
	uint32_t releaseTime;
} btnStruct;

extern uint8_t main_button;
extern uint16_t encoderCounter;
extern uint8_t encoder_scrolled;

extern uint8_t current_state;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void updateSettings();

#endif /* INC_BUTTONS_H_ */
