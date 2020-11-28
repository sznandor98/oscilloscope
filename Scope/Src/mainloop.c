/*
 * mainloop.c
 *
 *  Created on: Oct 19, 2020
 *      Author: sznandor
 */

#include "mainloop.h"

void initValues(){
    if(HAL_OK != HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_1))
    	Error_Handler();

    if(HAL_OK != HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1))
    	Error_Handler();

    if(HAL_OK != HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED))
    	Error_Handler();

    if(HAL_OK != HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED))
    	Error_Handler();


    initDisplay();
    drawGui(_screen);
  	setTriggerSource(TRIGGER_SOURCE_CH1);
  	CH1_setCoupling(CHANNEL_DC);
  	CH2_setCoupling(CHANNEL_DC);
  	CH1_selectADC(ADC_MCU);
  	setTriggerLevel(1.67f);

  	disableExtADC();
  	HAL_GPIO_WritePin(VSYNC_GPIO_Port, VSYNC_Pin, GPIO_PIN_SET);
  	HAL_GPIO_WritePin(HSYNC_GPIO_Port, HSYNC_Pin, GPIO_PIN_SET);
}


void scope_run(){
	initValues();
	enableFrequencyMeasurement();
  	while (1)
  	{
  		State_Task();
  		Display_Task();
  		Channels_Task();
  		DCMI_Task();
  	}
}
