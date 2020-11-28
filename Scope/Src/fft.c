/*
 * signal_processing.c
 *
 *  Created on: Oct 26, 2020
 *      Author: sznandor
 */


#include "fft.h"

uint16_t fftSize = ADC_BUF_LEN;
uint8_t ifftFlag = 0;
uint8_t doBitReverse = 1;
uint32_t fft_maxIndex;
float32_t fft_maxValue;

float32_t fft_output[ADC_BUF_LEN/2];

float FFT_freq_lookup[] = {165.1f, 121.1f, 86.5f, 55.0f, 32.4f, 18.0f, 7.1f, 2.8f};

float32_t complexABS(float32_t real, float32_t compl) {
	return sqrtf(real*real+compl*compl);
}

void calculateFFT(uint8_t channel_id){
	arm_status status;
	arm_rfft_fast_instance_f32 S;

	float32_t fft_input[fftSize];
	float32_t fft_output_temp[fftSize];
	for(uint32_t i = 0; i < fftSize; i++){
		fft_input[i] = (float32_t)((float32_t)channels[channel_id].buff[i]-128);
	}

	status = ARM_MATH_SUCCESS;

	/* Initialize the RFFT module */
	status = arm_rfft_fast_init_f32(&S, fftSize);

	/* Process the data through the RFFT module */
	arm_rfft_fast_f32(&S, fft_input, fft_output_temp, ifftFlag);

	/* Process the data through the Complex Magnitude Module for
	calculating the magnitude at each bin */

	uint16_t freqpoint = 0;
	uint16_t offset = 43; //variable noise floor offset

	//calculate abs values and linear-to-dB
	for (uint16_t i=0; i<fftSize; i=i+2) {
		fft_output[freqpoint] = (uint16_t)(20*log10f(complexABS(fft_output_temp[i], fft_output_temp[i+1])))-offset;
		if (fft_output[freqpoint]<0)
			fft_output[freqpoint]=0;
		freqpoint++;
	}

	/* Calculates maxValue and returns corresponding BIN value */
	arm_max_f32(fft_output, ADC_BUF_LEN/2, &fft_maxValue, &fft_maxIndex);
}
