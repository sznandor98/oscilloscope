/*
 * signal_processing.h
 *
 *  Created on: Oct 26, 2020
 *      Author: sznandor
 */

#ifndef INC_FFT_H_
#define INC_FFT_H_

#define ARM_MATH_CM4
#define __FPU_PRESENT 1
#include "arm_math.h"

#include "channels.h"

extern float32_t fft_output[];
extern float FFT_freq_lookup[];
extern uint16_t fftSize;
extern uint8_t ifftFlag;
extern uint8_t doBitReverse;
extern uint32_t fft_maxIndex;
extern float32_t fft_maxValue;

void calculateFFT(uint8_t channel_id);

#endif /* INC_FFT_H_ */
