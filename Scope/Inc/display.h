/*
 * display.h
 *
 *  Created on: Sep 29, 2020
 *      Author: sznandor
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "buttons.h"
#include "fft.h"
#include "ili9341.h"
#include "ili9341_gfx.h"
#include "ili9341_font.h"
#include "spi.h"
#include "channels.h"
#include <stdio.h>

#define ILI9341_SCREEN_WIDTH 320
#define ILI9341_SCREEN_HEIGHT 240

#define TRACE_AREA_WIDTH 310
#define TRACE_AREA_HEIGHT 190
#define TRACE_AREA_XOFFSET 10
#define TRACE_AREA_YOFFSET 0

#define ILI9341_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

#define TRACE_AREA_COLOR2 ILI9341_COLOR565(58, 210, 255)
#define TRACE_AREA_COLOR ILI9341_BLACK
#define FFT_COLOR		ILI9341_GREEN

extern uint8_t displayBusy;
extern ili9341_t *_screen;


ili9341_t *screen(void);
void screenTouchBegin(ili9341_t *lcd, uint16_t x, uint16_t y);
void screenTouchEnd(ili9341_t *lcd, uint16_t x, uint16_t y);
void initDisplay();
void clearTraceArea();
void drawGui();
void updateTrace(uint8_t channel_id, uint8_t step);
void deletePrevTrace(uint8_t channel_id);
void drawFFT();
void updateValues();
void updateTriggerIndicator();
void updateMenu(uint8_t state);

#endif /* INC_DISPLAY_H_ */
