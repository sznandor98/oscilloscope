/*
 * io.c
 *
 *  Created on: Sep 27, 2020
 *      Author: sznandor
 */

#include "buttons.h"

btnStruct btnTable[] = {
		{.pin = BTN1_Pin, .port = BTN1_GPIO_Port, .state = BTN_STATE_RELEASED, .isPressed = 0, .pressTime = 0, .releaseTime = 0},
		{.pin = BTN2_Pin, .port = BTN2_GPIO_Port, .state = BTN_STATE_RELEASED, .isPressed = 0, .pressTime = 0, .releaseTime = 0},
		{.pin = BTN3_Pin, .port = BTN3_GPIO_Port, .state = BTN_STATE_RELEASED, .isPressed = 0, .pressTime = 0, .releaseTime = 0},
		{.pin = BTN4_Pin, .port = BTN4_GPIO_Port, .state = BTN_STATE_RELEASED, .isPressed = 0, .pressTime = 0, .releaseTime = 0},
		{.pin = ENC_BTN_Pin, .port = ENC_BTN_GPIO_Port, .state = BTN_STATE_RELEASED, .isPressed = 0, .pressTime = 0, .releaseTime = 0}
};

uint16_t CNT_PREV = 0;
uint8_t encoder_scrolled = ENCODER_NOT_SCROLLED;
uint8_t main_button = PROCESSED;

uint8_t current_state = STATE_TIMEBASE;

void handlePress(btnStruct* btn) {
	if (HAL_GPIO_ReadPin(btn->port, btn->pin) == GPIO_PIN_SET) {
		if (HAL_GetTick() - btn->pressTime > DEBOUNCE_TIME_MS && HAL_GetTick() - btn->releaseTime > DEBOUNCE_TIME_MS) {
			btn->pressTime = HAL_GetTick();
			btn->state = BTN_STATE_PRESSED;
			btn->isPressed = 1;
		}
	}
}
void handleRelease(btnStruct* btn) {
	if (HAL_GPIO_ReadPin(btn->port, btn->pin) == GPIO_PIN_RESET) {
		if (HAL_GetTick() - btn->pressTime > DEBOUNCE_TIME_MS && HAL_GetTick() - btn->releaseTime > DEBOUNCE_TIME_MS)
		{
			btn->releaseTime = HAL_GetTick();
			btn->state = BTN_STATE_RELEASED;
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	for(uint8_t i = 0; i < BTN_NUM; i++){
		if(GPIO_Pin == btnTable[i].pin){
			handlePress(&btnTable[i]);
			handleRelease(&btnTable[i]);
		}
	}
}

void updateSettings(){
	for(int i = 0; i < BTN_NUM; i++){
		if(btnTable[i].isPressed == 1){
			if(btnTable[i].pin == BTN1_Pin){
				current_state = STATE_CHANNELS;
			}
			if(btnTable[i].pin == BTN2_Pin){
				current_state = STATE_TIMEBASE;
			}
			if(btnTable[i].pin == BTN3_Pin){
				current_state = STATE_DIV;
			}
			if(btnTable[i].pin == BTN4_Pin){
				current_state = STATE_TRIGGER;
			}
			if(btnTable[i].pin == ENC_BTN_Pin){
				main_button = PRESSED;
			}
			btnTable[i].isPressed = 0;
		}
	}
	if(CNT_PREV != TIM4->CNT){
		if(TIM4->CNT < CNT_PREV){
			encoder_scrolled = ENCODER_SCROLLED_DOWN;
		}
		if(TIM4->CNT > CNT_PREV){
			encoder_scrolled = ENCODER_SCROLLED_UP;
		}
		CNT_PREV = TIM4->CNT;
	}
}
