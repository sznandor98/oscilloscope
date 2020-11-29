/*
 * display.c
 *
 *  Created on: Sep 29, 2020
 *      Author: sznandor
 */

#include "display.h"



uint16_t delete_x[2][320];
uint16_t delete_y[2][320];


uint8_t displayBusy = 0;
uint16_t prevtriggers[] = {0, 0};
uint8_t prevState = STATE_DIV;
uint8_t prevChosen = CH2_ID;

float32_t vpps[2];

ili9341_t *_screen;

ili9341_t *screen(void)
{
  return _screen;
}

void screenTouchBegin(ili9341_t *lcd, uint16_t x, uint16_t y)
{
	if(x > 138 && x < 190 && y > 228 && y < 240){
		current_state = STATE_CHANNELS;
	}
	if(x > 198 && x < 249 && y > 228 && y < 240){
		current_state = STATE_TIMEBASE;
	}
	if(x > 258 && x < 280 && y > 228 && y < 240){
		current_state = STATE_DIV;
	}
	if(x > 290 && x < 320 && y > 228 && y < 240){
		current_state = STATE_TRIGGER;
	}
	if(x > TRACE_AREA_XOFFSET && x < TRACE_AREA_WIDTH && y > TRACE_AREA_YOFFSET && y < TRACE_AREA_HEIGHT){
		if(channels[CH1_ID].state == CHANNEL_CHOSEN){
			channels[CH1_ID].trig_lvl = y;
		}
		else if(channels[CH2_ID].state == CHANNEL_CHOSEN){
			channels[CH2_ID].trig_lvl = y;
		}
	}
}

void screenTouchEnd(ili9341_t *lcd, uint16_t x, uint16_t y)
{
  ;
}

void initDisplay(){
    _screen = ili9341_new(
        &hspi1,
        LCD_RST_GPIO_Port, LCD_RST_Pin,
        LCD_CS_GPIO_Port,    LCD_CS_Pin,
        LCD_DC_GPIO_Port,    LCD_DC_Pin,
        isoLandscapeFlip,
        0,  0,
        0, 0,
        0,
        itnNormalized);
}

void clearTraceArea(){
	ili9341_fill_rect(_screen, TRACE_AREA_COLOR, TRACE_AREA_XOFFSET, TRACE_AREA_YOFFSET, TRACE_AREA_WIDTH, TRACE_AREA_HEIGHT);
}

void drawGui(){
	displayBusy = 1;
	ili9341_fill_screen(_screen, ILI9341_WHITE);
	ili9341_fill_rect(_screen, TRACE_AREA_COLOR, TRACE_AREA_XOFFSET, TRACE_AREA_YOFFSET, TRACE_AREA_WIDTH, TRACE_AREA_HEIGHT);
	ili9341_draw_line(_screen, ILI9341_BLACK, TRACE_AREA_XOFFSET-1, 0, TRACE_AREA_XOFFSET-1, TRACE_AREA_HEIGHT);
	ili9341_draw_line(_screen, ILI9341_BLACK, TRACE_AREA_XOFFSET-1, TRACE_AREA_HEIGHT, ILI9341_SCREEN_WIDTH, TRACE_AREA_HEIGHT);
	ili9341_draw_line(_screen, ILI9341_BLACK, 0, TRACE_AREA_YOFFSET+23+TRACE_AREA_HEIGHT,
			ILI9341_SCREEN_WIDTH, TRACE_AREA_YOFFSET+23+TRACE_AREA_HEIGHT);

	// draw 10 x 8 grid to green area
	uint8_t i;
	uint8_t grid_diff = TRACE_AREA_WIDTH/10;
	for(i = 1; i < 10; i++){
		ili9341_draw_line(_screen, ILI9341_BLACK, TRACE_AREA_XOFFSET+i*grid_diff, TRACE_AREA_HEIGHT, TRACE_AREA_XOFFSET+i*grid_diff, TRACE_AREA_YOFFSET+TRACE_AREA_HEIGHT+10);
	}
	grid_diff = (TRACE_AREA_HEIGHT/8)+1;
	for(i = 1; i < 8; i++){
		ili9341_draw_line(_screen, ILI9341_BLACK, 0, i*grid_diff, TRACE_AREA_XOFFSET, i*grid_diff);
	}

	ili9341_text_attr_t txt = {.bg_color = ILI9341_RED, .fg_color = ILI9341_WHITE, .font = &ili9341_font_7x10, .origin_x = 0,
			.origin_y = 0
	};

	// CH1
	// print Vpp
	txt.origin_x = 2;
	txt.origin_y = 216;
	ili9341_draw_string(_screen, txt, "Vpp:    V");

	// print freq
	txt.origin_x = 67;
	txt.origin_y = 216;
	ili9341_draw_string(_screen, txt, "f:     ");

	// print div
	txt.origin_x = 5;
	txt.origin_y = 202;
	ili9341_draw_string(_screen, txt, "    V/D");

	//CH2
	txt.bg_color = ILI9341_BLUE;
	// print Vpp
	txt.origin_x = 2;
	txt.origin_y = 228;
	ili9341_draw_string(_screen, txt, "Vpp:    V");

	// print freq
	txt.origin_x = 67;
	txt.origin_y = 228;
	ili9341_draw_string(_screen, txt, "f:     ");

	// print div
	txt.origin_x = 160;
	txt.origin_y = 202;
	ili9341_draw_string(_screen, txt, "    V/D");
}

void updateTriggerIndicator(){
	// channel1
	displayBusy = 1;

	for(uint8_t i = 0; i < CHANNELS_NUM; i++){
		if(channels[i].state == CHANNEL_ACTIVE){
			if(prevtriggers[channels[i].id] != channels[i].trig_lvl){
				float32_t conv_val;
				uint16_t trig_val;

				conv_val = ((float32_t)prevtriggers[channels[i].id])/255.0f;
				trig_val = (uint16_t)(TRACE_AREA_HEIGHT - conv_val*TRACE_AREA_HEIGHT);
				ili9341_fill_rect(_screen, ILI9341_WHITE, 0, trig_val, 9, 5);

				conv_val = ((float32_t)channels[i].trig_lvl)/255.0f;
				trig_val = (uint16_t)(TRACE_AREA_HEIGHT - conv_val*TRACE_AREA_HEIGHT);
				ili9341_fill_rect(_screen, channels[i].color, 0, trig_val, 9, 5);

				uint8_t grid_diff = (TRACE_AREA_HEIGHT/8)+1;
				for(uint8_t i = 1; i < 8; i++){
					ili9341_draw_line(_screen, ILI9341_BLACK, 0, i*grid_diff, TRACE_AREA_XOFFSET, i*grid_diff);
				}
				prevtriggers[channels[i].id] = channels[i].trig_lvl;
			}
		}
	}
	displayBusy = 0;
}

void updateTrace(uint8_t channel_id, uint8_t step){
	uint16_t delete_x_tmp[310];
	uint16_t delete_y_tmp[310];
	displayBusy = 1;
	uint16_t max = 0;
	uint16_t min = 240;
	uint16_t trace_values[TRACE_AREA_WIDTH];

	for(uint32_t k = 0; k < TRACE_AREA_WIDTH; k++){
		float32_t conv_val = ((float32_t)channels[channel_id].buff[k + channels[channel_id].buffStart])/255.0f;
		trace_values[k] = (uint16_t)(TRACE_AREA_HEIGHT - conv_val*TRACE_AREA_HEIGHT);
		if(max < trace_values[k]){
			max = trace_values[k];
		}
		if(min > trace_values[k]){
			min = trace_values[k];
		}
	}
	if(channels[channel_id].adc == ADC_MCU){
		vpps[channel_id] = 1.25f*CH_DIVs[channels[channel_id].div_idx]*((float32_t)(max - min)/(float32_t)TRACE_AREA_HEIGHT)*3.3f;
	}
	else{
		vpps[channel_id] = 10.0f*CH_DIVs[channels[channel_id].div_idx]*((float32_t)(max - min)/(float32_t)TRACE_AREA_HEIGHT)*1.024f;
	}

	uint16_t x2 = 0;
	delete_x_tmp[0] = 0;
	uint16_t y2 = trace_values[0];
	delete_y_tmp[0] = y2;
	uint16_t x1 = step;
	for(uint16_t k = 1; k < TRACE_AREA_WIDTH; k++){
		uint16_t y1 = trace_values[k];
		uint16_t del_x1 = delete_x[channel_id][k];
		uint16_t del_y1 = delete_y[channel_id][k];
		uint16_t del_x2 = delete_x[channel_id][k-1];
		uint16_t del_y2 = delete_y[channel_id][k-1];
		ili9341_draw_line(_screen, TRACE_AREA_COLOR, TRACE_AREA_XOFFSET+del_x1, TRACE_AREA_YOFFSET+del_y1,
				TRACE_AREA_XOFFSET+del_x2, TRACE_AREA_YOFFSET+del_y2);
		ili9341_draw_line(_screen, channels[channel_id].color, TRACE_AREA_XOFFSET+x1, TRACE_AREA_YOFFSET+y1,
				TRACE_AREA_XOFFSET+x2, TRACE_AREA_YOFFSET+y2);

		delete_y_tmp[k] = y1;
		delete_x_tmp[k] = x1;
		x2 = x1;
		y2 = y1;
		x1 += step;
	}

	for(uint16_t k = 0; k < TRACE_AREA_WIDTH; k++){
		delete_x[channel_id][k] = delete_x_tmp[k];
		delete_y[channel_id][k] = delete_y_tmp[k];
	}

	displayBusy = 0;
}

void deletePrevTrace(uint8_t channel_id){
	displayBusy = 1;

	uint16_t x2 = delete_x[channel_id][0];
	uint16_t y2 = delete_y[channel_id][0];
	for(uint16_t i = 1; i < ILI9341_SCREEN_WIDTH; i++){
		uint16_t y1 = delete_y[channel_id][i];
		uint16_t x1 = delete_x[channel_id][i];
		ili9341_draw_line(_screen, TRACE_AREA_COLOR, TRACE_AREA_XOFFSET+x1, TRACE_AREA_YOFFSET+y1,
				TRACE_AREA_XOFFSET+x2, TRACE_AREA_YOFFSET+y2);
		x2 = x1;
		y2 = y1;
		x1 += 1;
	}
	displayBusy = 0;
}

void drawFFT(){
	displayBusy = 1;
	for(uint16_t i = 0; i < fftSize/2; i++){
		uint16_t bottomX = TRACE_AREA_XOFFSET+1*i;
		uint16_t bottomY = TRACE_AREA_HEIGHT;
		uint16_t height;
		float32_t value = fft_output[i];
		value++;
		height = (uint16_t)((float32_t)value / (float32_t)fft_maxValue * (float32_t)TRACE_AREA_HEIGHT);
		// delete prev line
    	ili9341_draw_line(_screen, TRACE_AREA_COLOR, bottomX, 0, bottomX, TRACE_AREA_HEIGHT);

	    if (height >= TRACE_AREA_HEIGHT) {
	    	ili9341_draw_line(_screen, FFT_COLOR, bottomX, 0, bottomX, TRACE_AREA_HEIGHT);
	    }
	    else if (height < TRACE_AREA_HEIGHT) {
	    	ili9341_draw_line(_screen, FFT_COLOR, bottomX, bottomY - height, bottomX, bottomY);
	    }
	}
	displayBusy = 0;
}

void updateValues(){
	displayBusy = 1;
	ili9341_text_attr_t txt = {.bg_color = ILI9341_RED, .fg_color = ILI9341_WHITE, .font = &ili9341_font_7x10, .origin_x = 0,
			.origin_y = 0
	};
	char BufferText[40];
	float32_t div;
	float32_t tb;
	char frequencyBuffer[40];
	for(uint8_t i = 0; i < CHANNELS_NUM; i++){
		if(channels[i].state == CHANNEL_ACTIVE){

			// set timebase var
			if(channels[CH1_ID].displayMode == DISPLAYMODE_TIME){
				if(channels[i].adc == ADC_EXT){
					tb = ExtADC_timebase_lookup[channels[i].sampling_idx];
				}
				else{
					tb = ADC_timebase_lookup[channels[i].sampling_idx];
				}
				snprintf(BufferText, sizeof(BufferText), "%.2fus/D", tb);
			}
			if(channels[i].displayMode == DISPLAYMODE_FFT){
				tb = FFT_freq_lookup[channels[i].sampling_idx];
				snprintf(BufferText, sizeof(BufferText), "%.1fkHz/D", tb);
			}
			// set div var
			if(channels[i].adc == ADC_MCU){
				div = 1.25f*CH_DIVs[channels[i].div_idx]*0.126f*3.3f;
			}
			else{
				div = 10.0f*CH_DIVs[channels[i].div_idx]*0.126f*1.024f;
			}

			// set frequency var
			if(channels[i].frequency > 999){
				__disable_irq();
				snprintf(frequencyBuffer, sizeof(frequencyBuffer), "%.2fMHz", channels[i].frequency/1000);
				__enable_irq();
			}
			else{
				__disable_irq();
				snprintf(frequencyBuffer, sizeof(frequencyBuffer), "%.2fkHz", channels[i].frequency);
				__enable_irq();
			}

			if(channels[i].id == CH1_ID){
				// CH1
				// print timebase
				txt.origin_x = 80;
				txt.origin_y = 202;
				ili9341_fill_rect(_screen, ILI9341_WHITE, 79, 201, 75, 11);
				ili9341_draw_string(_screen, txt, BufferText);

				// print Div
				snprintf(BufferText, sizeof(BufferText), "%.2f", div);
				txt.origin_x = 5;
				txt.origin_y = 202;
				ili9341_draw_string(_screen, txt, BufferText);

				// print Vpp
				snprintf(BufferText, sizeof(BufferText), "%.2f", vpps[channels[i].id]);
				txt.origin_x = 29;
				txt.origin_y = 216;
				ili9341_draw_string(_screen, txt, BufferText);

				// print freq
				txt.origin_x = 81;
				txt.origin_y = 216;
				ili9341_fill_rect(_screen, ILI9341_WHITE, 81, 216, 61, 11);
				ili9341_draw_string(_screen, txt, frequencyBuffer);
			}
			if(channels[i].id == CH2_ID){
				txt.bg_color = ILI9341_BLUE;
				// CH2
				// print timebase
				txt.origin_x = 230;
				txt.origin_y = 202;
				ili9341_fill_rect(_screen, ILI9341_WHITE, 249, 201, 75, 11);
				ili9341_draw_string(_screen, txt, BufferText);

				// print Div
				snprintf(BufferText, sizeof(BufferText), "%.2f", div);
				txt.origin_x = 160;
				txt.origin_y = 202;
				ili9341_draw_string(_screen, txt, BufferText);

				// print Vpp
				snprintf(BufferText, sizeof(BufferText), "%.2f", vpps[channels[i].id]);
				txt.origin_x = 29;
				txt.origin_y = 228;
				ili9341_draw_string(_screen, txt, BufferText);

				// print freq
				txt.origin_x = 81;
				txt.origin_y = 228;
				ili9341_fill_rect(_screen, ILI9341_WHITE, 81, 228, 56, 11);
				ili9341_draw_string(_screen, txt, frequencyBuffer);
			}
		}
	}
	displayBusy = 0;
}

void updateMenu(uint8_t state){
	displayBusy = 1;
	char BufferText[40];
	uint8_t chosen_channel = 3;
	if(channels[0].chosen == CHANNEL_CHOSEN){
		if(channels[0].adc == ADC_MCU){
			snprintf(BufferText, sizeof(BufferText), "CH:1 MCU");
			chosen_channel = CH1_ID;
		}
		else{
			snprintf(BufferText, sizeof(BufferText), "CH:1 EXT");
			chosen_channel = 5;
		}
	}
	else if(channels[1].chosen == CHANNEL_CHOSEN){
		snprintf(BufferText, sizeof(BufferText), "CH:2");
		chosen_channel = CH2_ID;
	}
	if(chosen_channel == 3){
		snprintf(BufferText, sizeof(BufferText), "CH:NONE");
	}
	if(chosen_channel != prevChosen){
		ili9341_text_attr_t txt = {.bg_color = ILI9341_WHITE, .fg_color = ILI9341_BLACK, .font = &ili9341_font_7x10,
				.origin_x = 145,
				.origin_y = 216
		};
		ili9341_fill_rect(_screen, ILI9341_WHITE, 138, 216, 160, 11);
		ili9341_draw_string(_screen, txt, BufferText);
		prevChosen = chosen_channel;
	}

	if(state != prevState){
		ili9341_text_attr_t txt = {.bg_color = ILI9341_BLACK, .fg_color = ILI9341_WHITE, .font = &ili9341_font_7x10, .origin_x = 0,
				.origin_y = 0
		};
		ili9341_fill_rect(_screen, ILI9341_WHITE, 138, 227, 182, 12);

		txt.origin_y = 228;
		if(state == STATE_CHANNELS){
			txt.bg_color = ILI9341_GREEN;
		}
		else{
			txt.bg_color = ILI9341_BLACK;
		}
		snprintf(BufferText, sizeof(BufferText), "Channels");
		txt.origin_x = 138;
		ili9341_draw_string(_screen, txt, BufferText);

		if(state == STATE_TIMEBASE){
			txt.bg_color = ILI9341_GREEN;
		}
		else{
			txt.bg_color = ILI9341_BLACK;
		}
		snprintf(BufferText, sizeof(BufferText), "Timebase");
		txt.origin_x = 198;
		ili9341_draw_string(_screen, txt, BufferText);

		if(state == STATE_DIV){
			txt.bg_color = ILI9341_GREEN;
		}
		else{
			txt.bg_color = ILI9341_BLACK;
		}
		snprintf(BufferText, sizeof(BufferText), "Gain");
		txt.origin_x = 258;
		ili9341_draw_string(_screen, txt, BufferText);

		if(state == STATE_TRIGGER){
			txt.bg_color = ILI9341_GREEN;
		}
		else{
			txt.bg_color = ILI9341_BLACK;
		}
		snprintf(BufferText, sizeof(BufferText), "Trig");
		txt.origin_x = 290;
		ili9341_draw_string(_screen, txt, BufferText);

		prevState = state;
	}

	displayBusy = 0;
}
