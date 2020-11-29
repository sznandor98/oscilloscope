/*
 * adc_custom.c
 *
 *  Created on: Oct 23, 2020
 *      Author: sznandor
 */

#include "channels.h"


uint8_t CH_DIVs[] = {CHANNEL_DIV_1, CHANNEL_DIV_2, CHANNEL_DIV_5, CHANNEL_DIV_10, CHANNEL_DIV_20, CHANNEL_DIV_50, CHANNEL_GND};

/*
uint32_t ADC_CLK_DIVs[] = {ADC_CLOCK_ASYNC_DIV2, ADC_CLOCK_ASYNC_DIV4, ADC_CLOCK_ASYNC_DIV6, ADC_CLOCK_ASYNC_DIV8,
		ADC_CLOCK_ASYNC_DIV10, ADC_CLOCK_ASYNC_DIV16, ADC_CLOCK_ASYNC_DIV32,
		ADC_CLOCK_ASYNC_DIV64, ADC_CLOCK_ASYNC_DIV128, ADC_CLOCK_ASYNC_DIV256};

*/
uint32_t ADC_SAMPLING_TIMES[] = {ADC_SAMPLETIME_2CYCLES_5, ADC_SAMPLETIME_6CYCLES_5, ADC_SAMPLETIME_12CYCLES_5,
		ADC_SAMPLETIME_24CYCLES_5, ADC_SAMPLETIME_47CYCLES_5, ADC_SAMPLETIME_92CYCLES_5, ADC_SAMPLETIME_247CYCLES_5,
		ADC_SAMPLETIME_640CYCLES_5};

uint32_t MCO_DIVs[] = {RCC_MCODIV_1, RCC_MCODIV_2, RCC_MCODIV_4, RCC_MCODIV_8, RCC_MCODIV_16};

float ADC_timebase_lookup[] = {5.68f, 7.75f, 10.85f, 17.05f, 28.93f, 52.18f, 132.27f, 335.32f};

float ExtADC_timebase_lookup[] = {0.65f, 1.29f, 2.58f, 5.17f, 10.34f};

channelStruct channels[] = {
		{.id = CH1_ID, .state = CHANNEL_ACTIVE, .chosen = CHANNEL_CHOSEN, .div_idx = 0, .div_changed = FALSE,
		.sampling_idx = 0, .sampling_changed = FALSE, .trig_lvl = 128, .trig_edge = CHANNEL_TRIGGER_RISING, .converting = FALSE,
		.triggered = FALSE, .coupling = CHANNEL_DC, .coupling_changed = FALSE, .buffSize = ADC_BUF_LEN,
		.bufferFull = FALSE, .displayMode = DISPLAYMODE_TIME, .color = CH1_COLOR, .adc = ADC_MCU},

		{.id = CH2_ID, .state = CHANNEL_INACTIVE, .chosen = CHANNEL_NOT_CHOSEN, .div_idx = 0, .div_changed = FALSE,
		.sampling_idx = 0, .sampling_changed = FALSE, .trig_lvl = 128, .trig_edge = CHANNEL_TRIGGER_RISING, .converting = FALSE,
		.triggered = FALSE, .coupling = CHANNEL_DC, .coupling_changed = FALSE, .buffSize = ADC_BUF_LEN,
		.bufferFull = FALSE, .displayMode = DISPLAYMODE_TIME, .color = CH2_COLOR, .adc = ADC_MCU}
};

uint8_t ok = 0;
uint32_t dcmi_buff[256];
uint32_t last_dcmi_time;
uint8_t mco_enabled = TRUE;

void resetDCMI(){
	HAL_GPIO_WritePin(VSYNC_GPIO_Port, VSYNC_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(HSYNC_GPIO_Port, HSYNC_Pin, GPIO_PIN_SET);
	//if(channels[0].converting == TRUE){
		//HAL_DCMI_Stop(&hdcmi);
	//}
	channels[0].converting = FALSE;
	channels[0].bufferFull = FALSE;
	HAL_DCMI_Stop(&hdcmi);
	last_dcmi_time = HAL_GetTick();
}

// called in DMA2_Channel1_IRQHandler()
void externalADCompleteCallback(){
	HAL_GPIO_WritePin(VSYNC_GPIO_Port, VSYNC_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(HSYNC_GPIO_Port, HSYNC_Pin, GPIO_PIN_SET);

	uint32_t j = 0;
	for(uint32_t i = 0; i < 256; i++){
		channels[0].buff[j] = (uint8_t)(dcmi_buff[i] & 0x000000FF);
		j++;
		channels[0].buff[j] = (uint8_t)((dcmi_buff[i] & 0x0000FF00) >> 8);
		j++;
		channels[0].buff[j] = (uint8_t)((dcmi_buff[i] & 0x00FF0000) >> 16);
		j++;
		channels[0].buff[j] = (uint8_t)((dcmi_buff[i] & 0xFF000000) >> 24);
		j++;
	}

	channels[0].converting = FALSE;
	channels[0].bufferFull = TRUE;
	HAL_DCMI_Stop(&hdcmi);
	last_dcmi_time = HAL_GetTick();
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC1){
		if(HAL_ADC_Stop_DMA(&hadc1) != HAL_OK)
			Error_Handler();
		channels[0].converting = FALSE;
		channels[0].bufferFull = TRUE;
	}

	if(hadc->Instance == ADC2){
		if(HAL_ADC_Stop_DMA(&hadc2) != HAL_OK)
			Error_Handler();
		channels[1].converting = FALSE;
		channels[1].bufferFull = TRUE;
	}
}

uint32_t IC_Val1;
uint32_t Frequency;
uint32_t last_freq_time;
uint8_t is_second;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim){
	if(is_second == 0){
		is_second = 1;
	}
	else if(is_second == 1){
		is_second = 0;
		last_freq_time = HAL_GetTick();
		disableFrequencyMeasurement();
	}
	__disable_irq();
	if(htim->Instance == TIM1){
		IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
		if(IC_Val1 != 0)
			Frequency = (HAL_RCC_GetPCLK1Freq()/IC_Val1);
		else
			Frequency = 0;


		if(channels[0].chosen == CHANNEL_CHOSEN){
			channels[0].frequency = (float32_t)Frequency/1000.0f;
		}
		else if(channels[1].chosen == CHANNEL_CHOSEN){
			channels[1].frequency = (float32_t)Frequency/1000.0f;
		}

	}
	__enable_irq();
}

void disableFrequencyMeasurement(){
	if(HAL_OK != HAL_TIM_IC_Stop_IT(&htim1, TIM_CHANNEL_2))
	    Error_Handler();
}
void enableFrequencyMeasurement(){
	if(HAL_OK != HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_2))
		Error_Handler();
}

void disableExtADC(){
	if(mco_enabled == TRUE){
		HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_NOCLOCK, RCC_MCODIV_1);
		mco_enabled = FALSE;
	}
	HAL_GPIO_WritePin(ADC_PWRDN_GPIO_Port, ADC_PWRDN_Pin, GPIO_PIN_SET);
}
void enableExtADC(){
	if(mco_enabled == FALSE){
		HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI48, RCC_MCODIV_1);
		mco_enabled = TRUE;
	}
	HAL_GPIO_WritePin(ADC_PWRDN_GPIO_Port, ADC_PWRDN_Pin, GPIO_PIN_RESET);
}

void findTrigger(uint8_t channel_id){
	for(channels[channel_id].buffStart = 0; channels[channel_id].buffStart < channels[channel_id].buffSize/2; channels[channel_id].buffStart++){
		if(channels[channel_id].trig_edge == CHANNEL_TRIGGER_RISING){
			if(channels[channel_id].buff[channels[channel_id].buffStart] <= channels[channel_id].trig_lvl){
				break;
			}
		}
		else{
			if(channels[channel_id].buff[channels[channel_id].buffStart] >= channels[channel_id].trig_lvl){
				break;
			}
		}
	}
	channels[channel_id].triggered = FALSE;
	for(; channels[channel_id].buffStart < channels[channel_id].buffSize/2; channels[channel_id].buffStart++){
		if(channels[channel_id].trig_edge == CHANNEL_TRIGGER_RISING){
			if(channels[channel_id].buff[channels[channel_id].buffStart] + channels[channel_id].buff[channels[channel_id].buffStart+1] >
					channels[channel_id].buff[channels[channel_id].buffStart-1] + channels[channel_id].buff[channels[channel_id].buffStart-2] &&
					channels[channel_id].buff[channels[channel_id].buffStart] > channels[channel_id].trig_lvl){
				channels[channel_id].triggered = TRUE;
				break;
			}
		}
		else{
			if(channels[channel_id].buff[channels[channel_id].buffStart] + channels[channel_id].buff[channels[channel_id].buffStart+1] <
					channels[channel_id].buff[channels[channel_id].buffStart-1] + channels[channel_id].buff[channels[channel_id].buffStart-2] &&
					channels[channel_id].buff[channels[channel_id].buffStart] < channels[channel_id].trig_lvl){
				channels[channel_id].triggered = TRUE;
				break;
			}
		}
	}
}

// change MCO prescaler
void ExtADC_Change_Sampling(uint32_t MCO_CLOCK_DIV){
	if(mco_enabled == TRUE)
		HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI48, MCO_CLOCK_DIV);
}

// Re Init ADC with given sampling time to change sampling rate
void ADC1_Change_Sampling(uint32_t ADC_SAMPLING_TIME)
{
	if (HAL_ADC_DeInit(&hadc1) != HAL_OK)
	{
		HAL_ADC_DeInit(&hadc1);
	}


  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_8B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLING_TIME;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

// Re Init ADC with given sampling time to change sampling rate
void ADC2_Change_Sampling(uint32_t ADC_SAMPLING_TIME)
{
	if (HAL_ADC_DeInit(&hadc2) != HAL_OK)
	{
		HAL_ADC_DeInit(&hadc2);
	}
  ADC_ChannelConfTypeDef sConfig = {0};

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
  hadc2.Init.Resolution = ADC_RESOLUTION_8B;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.ContinuousConvMode = ENABLE;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.DMAContinuousRequests = ENABLE;
  hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc2.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLING_TIME;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

// value in Volts between 0 and 3.3
void setTriggerLevel(float32_t value){
	if(value > 3.3f){
		value = 3.3f;
	}
	if(value < 0.0f){
		value = 0.0f;
	}
	htim2.Instance->CCR1 = (uint32_t)((100.0f * value / 3.3f) + 1.0f);
}

void CH1_setDiv(uint8_t div){
	switch(div){
		case CHANNEL_DIV_1:
			HAL_GPIO_WritePin(CH1_S0_GPIO_Port, CH1_S0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH1_S1_GPIO_Port, CH1_S1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH1_S2_GPIO_Port, CH1_S2_Pin, GPIO_PIN_RESET);
			break;
		case CHANNEL_DIV_2:
			HAL_GPIO_WritePin(CH1_S0_GPIO_Port, CH1_S0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(CH1_S1_GPIO_Port, CH1_S1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH1_S2_GPIO_Port, CH1_S2_Pin, GPIO_PIN_RESET);
			break;
		case CHANNEL_DIV_5:
			HAL_GPIO_WritePin(CH1_S0_GPIO_Port, CH1_S0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH1_S1_GPIO_Port, CH1_S1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(CH1_S2_GPIO_Port, CH1_S2_Pin, GPIO_PIN_RESET);
			break;
		case CHANNEL_DIV_10:
			HAL_GPIO_WritePin(CH1_S0_GPIO_Port, CH1_S0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(CH1_S1_GPIO_Port, CH1_S1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(CH1_S2_GPIO_Port, CH1_S2_Pin, GPIO_PIN_RESET);
			break;
		case CHANNEL_DIV_20:
			HAL_GPIO_WritePin(CH1_S0_GPIO_Port, CH1_S0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH1_S1_GPIO_Port, CH1_S1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH1_S2_GPIO_Port, CH1_S2_Pin, GPIO_PIN_SET);
			break;
		case CHANNEL_DIV_50:
			HAL_GPIO_WritePin(CH1_S0_GPIO_Port, CH1_S0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(CH1_S1_GPIO_Port, CH1_S1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH1_S2_GPIO_Port, CH1_S2_Pin, GPIO_PIN_SET);
			break;
		case CHANNEL_GND:
			HAL_GPIO_WritePin(CH1_S0_GPIO_Port, CH1_S0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH1_S1_GPIO_Port, CH1_S1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(CH1_S2_GPIO_Port, CH1_S2_Pin, GPIO_PIN_SET);
			break;
		default:
			HAL_GPIO_WritePin(CH1_S0_GPIO_Port, CH1_S0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH1_S1_GPIO_Port, CH1_S1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(CH1_S2_GPIO_Port, CH1_S2_Pin, GPIO_PIN_SET);
	}
}

void CH2_setDiv(uint8_t div){
	switch(div){
		case CHANNEL_DIV_1:
			HAL_GPIO_WritePin(CH2_S0_GPIO_Port, CH2_S0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH2_S1_GPIO_Port, CH2_S1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH2_S2_GPIO_Port, CH2_S2_Pin, GPIO_PIN_RESET);
			break;
		case CHANNEL_DIV_2:
			HAL_GPIO_WritePin(CH2_S0_GPIO_Port, CH2_S0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(CH2_S1_GPIO_Port, CH2_S1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH2_S2_GPIO_Port, CH2_S2_Pin, GPIO_PIN_RESET);
			break;
		case CHANNEL_DIV_5:
			HAL_GPIO_WritePin(CH2_S0_GPIO_Port, CH2_S0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH2_S1_GPIO_Port, CH2_S1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(CH2_S2_GPIO_Port, CH2_S2_Pin, GPIO_PIN_RESET);
			break;
		case CHANNEL_DIV_10:
			HAL_GPIO_WritePin(CH2_S0_GPIO_Port, CH2_S0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(CH2_S1_GPIO_Port, CH2_S1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(CH2_S2_GPIO_Port, CH2_S2_Pin, GPIO_PIN_RESET);
			break;
		case CHANNEL_DIV_20:
			HAL_GPIO_WritePin(CH2_S0_GPIO_Port, CH2_S0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH2_S1_GPIO_Port, CH2_S1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH2_S2_GPIO_Port, CH2_S2_Pin, GPIO_PIN_SET);
			break;
		case CHANNEL_DIV_50:
			HAL_GPIO_WritePin(CH2_S0_GPIO_Port, CH2_S0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(CH2_S1_GPIO_Port, CH2_S1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH2_S2_GPIO_Port, CH2_S2_Pin, GPIO_PIN_SET);
			break;
		case CHANNEL_GND:
			HAL_GPIO_WritePin(CH2_S0_GPIO_Port, CH2_S0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH2_S1_GPIO_Port, CH2_S1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(CH2_S2_GPIO_Port, CH2_S2_Pin, GPIO_PIN_SET);
			break;
		default:
			HAL_GPIO_WritePin(CH2_S0_GPIO_Port, CH2_S0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CH2_S1_GPIO_Port, CH2_S1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(CH2_S2_GPIO_Port, CH2_S2_Pin, GPIO_PIN_SET);
	}
}

void setTriggerSource(uint8_t source){
	switch(source){
		case TRIGGER_SOURCE_CH1:
		  	HAL_GPIO_WritePin(TRIG_SEL0_GPIO_Port, TRIG_SEL0_Pin, GPIO_PIN_SET);
		  	HAL_GPIO_WritePin(TRIG_SEL1_GPIO_Port, TRIG_SEL1_Pin, GPIO_PIN_RESET);
			break;
		case TRIGGER_SOURCE_CH2:
		  	HAL_GPIO_WritePin(TRIG_SEL0_GPIO_Port, TRIG_SEL0_Pin, GPIO_PIN_RESET);
		  	HAL_GPIO_WritePin(TRIG_SEL1_GPIO_Port, TRIG_SEL1_Pin, GPIO_PIN_SET);
			break;
		case TRIGGER_SOURCE_EXTADC:
		  	HAL_GPIO_WritePin(TRIG_SEL0_GPIO_Port, TRIG_SEL0_Pin, GPIO_PIN_RESET);
		  	HAL_GPIO_WritePin(TRIG_SEL1_GPIO_Port, TRIG_SEL1_Pin, GPIO_PIN_RESET);
			break;
		default:
		  	HAL_GPIO_WritePin(TRIG_SEL0_GPIO_Port, TRIG_SEL0_Pin, GPIO_PIN_SET);
		  	HAL_GPIO_WritePin(TRIG_SEL1_GPIO_Port, TRIG_SEL1_Pin, GPIO_PIN_RESET);
	}
}

void CH1_setCoupling(uint8_t mode){
	if(mode == CHANNEL_AC){
	  	HAL_GPIO_WritePin(CH1_ACDC_GPIO_Port, CH1_ACDC_Pin, GPIO_PIN_RESET);
	}
	if(mode == CHANNEL_DC){
	  	HAL_GPIO_WritePin(CH1_ACDC_GPIO_Port, CH1_ACDC_Pin, GPIO_PIN_SET);
	}
}

void CH2_setCoupling(uint8_t mode){
	if(mode == CHANNEL_AC){
	  	HAL_GPIO_WritePin(CH2_ACDC_GPIO_Port, CH2_ACDC_Pin, GPIO_PIN_RESET);
	}
	if(mode == CHANNEL_DC){
	  	HAL_GPIO_WritePin(CH2_ACDC_GPIO_Port, CH2_ACDC_Pin, GPIO_PIN_SET);
	}
}

void CH1_selectADC(uint8_t adc){
	if(adc == ADC_MCU){
		HAL_GPIO_WritePin(CH1_MODE_GPIO_Port, CH1_MODE_Pin, GPIO_PIN_SET);
	}
	if(adc == ADC_EXT){
		HAL_GPIO_WritePin(CH1_MODE_GPIO_Port, CH1_MODE_Pin, GPIO_PIN_RESET);
	}
}

void startConversion(){
	if(channels[0].converting == FALSE && channels[0].bufferFull == FALSE && channels[0].state == CHANNEL_ACTIVE){
		if(channels[0].adc == ADC_MCU){
			channels[0].converting = TRUE;
	  		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)channels[0].buff, channels[0].buffSize);
		}
		else{
			channels[0].converting = TRUE;
			HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)dcmi_buff, 256);
	  		HAL_GPIO_WritePin(VSYNC_GPIO_Port, VSYNC_Pin, GPIO_PIN_RESET);
	  		HAL_GPIO_WritePin(HSYNC_GPIO_Port, HSYNC_Pin, GPIO_PIN_RESET);
		}
	}
	if(channels[1].converting == FALSE && channels[1].bufferFull == FALSE && channels[1].state == CHANNEL_ACTIVE){
		channels[1].converting = TRUE;
		HAL_ADC_Start_DMA(&hadc2, (uint32_t*)channels[1].buff, channels[1].buffSize);
	}
}

void updateChannels(){
	for(uint8_t i = 0; i < CHANNELS_NUM; i++){
		if(channels[i].state == CHANNEL_ACTIVE){
			if(channels[i].div_changed == TRUE){

				if(channels[i].id == CH1_ID){
					CH1_setDiv(CH_DIVs[channels[i].div_idx]);
				}
				else{
					CH2_setDiv(CH_DIVs[channels[i].div_idx]);
				}
				channels[i].div_changed = FALSE;
			}

			if(channels[i].coupling_changed == TRUE){
				if(channels[i].id == CH1_ID){
					CH1_setCoupling(channels[i].coupling);
				}
				else{
					CH2_setCoupling(channels[i].coupling);
				}
				channels[i].coupling_changed = FALSE;
			}

			if(channels[i].sampling_changed == TRUE){
				if(channels[i].id == CH1_ID){
					if(channels[i].adc == ADC_EXT){
						resetDCMI();
						ExtADC_Change_Sampling(MCO_DIVs[channels[i].sampling_idx]);
					}
					else{
						if(HAL_ADC_Stop_DMA(&hadc1) != HAL_OK){
							Error_Handler();
						}
						ADC1_Change_Sampling(ADC_SAMPLING_TIMES[channels[i].sampling_idx]);
					}
				}
				else{
					if(HAL_ADC_Stop_DMA(&hadc2) != HAL_OK){
						Error_Handler();
					}
					ADC2_Change_Sampling(ADC_SAMPLING_TIMES[channels[i].sampling_idx]);
				}

				channels[i].triggered = FALSE;

				channels[i].bufferFull = FALSE;
				channels[i].converting = FALSE;
				channels[i].sampling_changed = FALSE;
			}
		}
	}
}
