/*
 * tasks.c
 *
 *  Created on: Nov 20, 2020
 *      Author: sznandor
 */

#include "tasks.h"

uint32_t display_timer;
uint8_t channel_state_counter;
uint8_t dual_channel_mode;

void State_Task(){
	updateSettings();
	for(uint8_t i = 0; i < CHANNELS_NUM; i++){
		if(current_state == STATE_DIV){
			if(encoder_scrolled == ENCODER_SCROLLED_UP){
				if(channels[i].chosen == CHANNEL_CHOSEN){
					if(channels[i].div_idx < 6){
						channels[i].div_idx++;
						channels[i].div_changed = TRUE;
					}
				}
			}
			else if(encoder_scrolled == ENCODER_SCROLLED_DOWN){
				if(channels[i].chosen == CHANNEL_CHOSEN){
					if(channels[i].div_idx != 0){
						channels[i].div_idx--;
						channels[i].div_changed = TRUE;
					}
				}
			}
			if(main_button == PRESSED){
				if(channels[i].chosen == CHANNEL_CHOSEN){
					if(channels[i].coupling == CHANNEL_DC){
						channels[i].coupling = CHANNEL_AC;
						channels[i].coupling_changed = TRUE;
					}
					else{
						channels[i].coupling = CHANNEL_DC;
						channels[i].coupling_changed = TRUE;
					}
				}
			}
		}
		if(current_state == STATE_TIMEBASE){
			if(encoder_scrolled == ENCODER_SCROLLED_UP){
				if(channels[i].chosen == CHANNEL_CHOSEN){
					if(channels[i].adc == ADC_EXT){
						if(channels[i].sampling_idx < 4){
							channels[i].sampling_idx++;
						}
					}
					else{
						if(channels[i].sampling_idx < 7){
							channels[i].sampling_idx++;
						}
					}
					channels[i].sampling_changed = TRUE;
				}
			}
			else if(encoder_scrolled == ENCODER_SCROLLED_DOWN){
				if(channels[i].chosen == CHANNEL_CHOSEN){
					if(channels[i].sampling_idx != 0){
						channels[i].sampling_idx--;
						channels[i].sampling_changed = TRUE;
					}
				}
			}
			if(main_button == PRESSED){
				if(channels[i].displayMode == DISPLAYMODE_TIME){
					channels[i].displayMode = DISPLAYMODE_FFT;
					deletePrevTrace(channels[i].id);
				}
				else{
					channels[i].displayMode = DISPLAYMODE_TIME;
					clearTraceArea();
				}
			}
		}

		if(current_state == STATE_TRIGGER){
			if(encoder_scrolled == ENCODER_SCROLLED_UP){
				if(channels[i].chosen == CHANNEL_CHOSEN){
					if(channels[i].trig_lvl < 250){
						channels[i].trig_lvl += 5;
						float32_t trigval;
						if(channels[i].adc == ADC_EXT){
							trigval = (float32_t)((channels[i].trig_lvl/255.0f)*1.024f+0.612f);
						}
						else{
							trigval = (float32_t)(channels[i].trig_lvl/255.0f)*3.3f;
						}
						setTriggerLevel(trigval);
					}
				}
			}
			else if(encoder_scrolled == ENCODER_SCROLLED_DOWN){
				if(channels[i].chosen == CHANNEL_CHOSEN){
					if(channels[i].trig_lvl > 5){
						channels[i].trig_lvl -= 5;
						float32_t trigval;
						if(channels[i].adc == ADC_EXT){
							trigval = (float32_t)((channels[i].trig_lvl/255.0f)*1.024f+0.512f);
						}
						else{
							trigval = (float32_t)(channels[i].trig_lvl/255.0f)*3.3f;
						}
						setTriggerLevel(trigval);
					}
				}
			}
			if(main_button == PRESSED){
				if(channels[i].chosen == CHANNEL_CHOSEN){
					if(channels[i].trig_edge == CHANNEL_TRIGGER_RISING)
						channels[i].trig_edge = CHANNEL_TRIGGER_FALLING;
					else
						channels[i].trig_edge = CHANNEL_TRIGGER_RISING;
				}
			}
		}
	}

	if(current_state == STATE_CHANNELS){
		uint8_t scrolled = 0;
		if(encoder_scrolled == ENCODER_SCROLLED_UP){
			channel_state_counter++;
			scrolled = 1;
		}
		else if(encoder_scrolled == ENCODER_SCROLLED_DOWN){
			channel_state_counter--;
			scrolled = 1;
		}
		if(dual_channel_mode == 0){
			if(scrolled == 1){
				scrolled = 0;
				channel_state_counter = channel_state_counter % 3;
				// ch1 mcu adc
				if(channel_state_counter == 0){
					deletePrevTrace(channels[1].id);
					channels[CH1_ID].state = CHANNEL_ACTIVE;
					channels[CH2_ID].state = CHANNEL_INACTIVE;
					channels[CH1_ID].adc = ADC_MCU;
					channels[CH1_ID].chosen = CHANNEL_CHOSEN;
					channels[CH2_ID].chosen = CHANNEL_NOT_CHOSEN;
					setTriggerSource(TRIGGER_SOURCE_CH1);
					CH1_selectADC(ADC_MCU);
					disableExtADC();
				}
				//ch1 ext adc

				if(channel_state_counter == 1){
					deletePrevTrace(channels[1].id);
					channels[CH1_ID].state = CHANNEL_ACTIVE;
					channels[CH2_ID].state = CHANNEL_INACTIVE;
					channels[CH1_ID].adc = ADC_EXT;
					channels[CH1_ID].chosen = CHANNEL_CHOSEN;
					channels[CH2_ID].chosen = CHANNEL_NOT_CHOSEN;
					setTriggerSource(TRIGGER_SOURCE_EXTADC);
					CH1_selectADC(ADC_EXT);
					enableExtADC();
				}

				// ch2
				if(channel_state_counter == 2){
					deletePrevTrace(channels[0].id);
					channels[CH2_ID].state = CHANNEL_ACTIVE;
					channels[CH1_ID].state = CHANNEL_INACTIVE;
					channels[CH2_ID].chosen = CHANNEL_CHOSEN;
					channels[CH1_ID].chosen = CHANNEL_NOT_CHOSEN;
					setTriggerSource(TRIGGER_SOURCE_CH2);
					disableExtADC();
				}
			}
		}
		else{
			if(scrolled == 1){
				scrolled = 0;
				channel_state_counter = channel_state_counter % 2;
				// ch1 mcu adc
				if(channel_state_counter == 0){
					channels[CH1_ID].chosen = CHANNEL_CHOSEN;
					channels[CH2_ID].chosen = CHANNEL_NOT_CHOSEN;
					setTriggerSource(TRIGGER_SOURCE_CH1);
				}
				// ch2
				if(channel_state_counter == 1){
					channels[CH2_ID].chosen = CHANNEL_CHOSEN;
					channels[CH1_ID].chosen = CHANNEL_NOT_CHOSEN;
					setTriggerSource(TRIGGER_SOURCE_CH2);
				}
			}
		}

		if(main_button == PRESSED){
			if(dual_channel_mode == 0){
				dual_channel_mode = 1;
				channels[CH2_ID].state = CHANNEL_ACTIVE;
				channels[CH1_ID].state = CHANNEL_ACTIVE;
				channels[CH1_ID].converting = FALSE;
				channels[CH2_ID].converting = FALSE;
				channels[CH1_ID].adc = ADC_MCU;
				channels[CH1_ID].chosen = CHANNEL_CHOSEN;
				channels[CH2_ID].chosen = CHANNEL_NOT_CHOSEN;
				setTriggerSource(TRIGGER_SOURCE_CH1);
				CH1_selectADC(ADC_MCU);
				disableExtADC();

			}
			else{
				dual_channel_mode = 0;
				deletePrevTrace(channels[1].id);
				channels[CH1_ID].state = CHANNEL_ACTIVE;
				channels[CH2_ID].state = CHANNEL_INACTIVE;
				channels[CH1_ID].adc = ADC_MCU;
				channels[CH1_ID].chosen = CHANNEL_CHOSEN;
				channels[CH2_ID].chosen = CHANNEL_NOT_CHOSEN;
				setTriggerSource(TRIGGER_SOURCE_CH2);
				CH1_selectADC(ADC_MCU);
				disableExtADC();
			}
		}
	}
	encoder_scrolled = ENCODER_NOT_SCROLLED;
	main_button = PROCESSED;
}

void Channels_Task(){
	updateChannels();
	startConversion();
	if(HAL_GetTick() - last_freq_time > 800){
		enableFrequencyMeasurement();
	}

}

void Display_Task(){
	if(HAL_GetTick() - display_timer > 800){
		updateValues();
		display_timer = HAL_GetTick();
	}

	updateMenu(current_state);
	updateTriggerIndicator();
	if(channels[CH1_ID].state == CHANNEL_ACTIVE){
		if(channels[CH1_ID].bufferFull == TRUE){
			if(channels[CH1_ID].displayMode == DISPLAYMODE_TIME){
		  		findTrigger(channels[CH1_ID].id);
				updateTrace(channels[CH1_ID].id, 1);
			}
			else{
	  			calculateFFT(channels[CH1_ID].id);
	  			drawFFT();
			}
			channels[CH1_ID].bufferFull = FALSE;
		}
	}

	if(channels[CH2_ID].state == CHANNEL_ACTIVE){
		if(channels[CH2_ID].bufferFull == TRUE){
			if(channels[CH2_ID].displayMode == DISPLAYMODE_TIME){
		  		findTrigger(channels[CH2_ID].id);
		  		updateTrace(channels[CH2_ID].id, 1);
			}
			else{
		  		calculateFFT(channels[CH2_ID].id);
		  		drawFFT();
			}
			channels[CH2_ID].bufferFull = FALSE;
		}
	}
}

void DCMI_Task(){
	if(channels[0].state == CHANNEL_ACTIVE && channels[0].adc == ADC_EXT && (HAL_GetTick() - last_dcmi_time > 500)){
		resetDCMI();
	}
}
