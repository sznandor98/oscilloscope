/*
 * tasks.h
 *
 *  Created on: Nov 20, 2020
 *      Author: sznandor
 */

#ifndef INC_TASKS_H_
#define INC_TASKS_H_

#include "buttons.h"
#include "display.h"
#include "channels.h"
#include "fft.h"

void State_Task();
void Display_Task();
void Channels_Task();
void DCMI_Task();

#endif /* INC_TASKS_H_ */
