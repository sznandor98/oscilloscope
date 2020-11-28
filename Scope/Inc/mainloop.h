/*
 * mainloop.h
 *
 *  Created on: Oct 19, 2020
 *      Author: sznandor
 */

#ifndef INC_MAINLOOP_H_
#define INC_MAINLOOP_H_

#include "buttons.h"
#include "fft.h"
#include "channels.h"
#include "display.h"
#include "tasks.h"
#include "main.h"
#include "adc.h"
#include "dcmi.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"


void scope_run();

extern ili9341_t *_screen;
extern uint8_t state;


#endif /* INC_MAINLOOP_H_ */
