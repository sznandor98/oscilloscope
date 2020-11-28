/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADC1_Pin GPIO_PIN_0
#define ADC1_GPIO_Port GPIOC
#define TS_DIN_Pin GPIO_PIN_1
#define TS_DIN_GPIO_Port GPIOC
#define TS_DO_Pin GPIO_PIN_2
#define TS_DO_GPIO_Port GPIOC
#define ADC2_Pin GPIO_PIN_3
#define ADC2_GPIO_Port GPIOC
#define LCD_CS_Pin GPIO_PIN_0
#define LCD_CS_GPIO_Port GPIOB
#define TRIG_SEL0_Pin GPIO_PIN_1
#define TRIG_SEL0_GPIO_Port GPIOB
#define TRIG_SEL1_Pin GPIO_PIN_2
#define TRIG_SEL1_GPIO_Port GPIOB
#define HSYNC_Pin GPIO_PIN_9
#define HSYNC_GPIO_Port GPIOE
#define VSYNC_Pin GPIO_PIN_13
#define VSYNC_GPIO_Port GPIOE
#define CH1_S2_Pin GPIO_PIN_15
#define CH1_S2_GPIO_Port GPIOE
#define T_SCK_Pin GPIO_PIN_10
#define T_SCK_GPIO_Port GPIOB
#define CH2_S2_Pin GPIO_PIN_11
#define CH2_S2_GPIO_Port GPIOB
#define T_CS_Pin GPIO_PIN_12
#define T_CS_GPIO_Port GPIOB
#define T_IRQ_Pin GPIO_PIN_13
#define T_IRQ_GPIO_Port GPIOB
#define CH2_S0_Pin GPIO_PIN_14
#define CH2_S0_GPIO_Port GPIOB
#define CH2_S1_Pin GPIO_PIN_15
#define CH2_S1_GPIO_Port GPIOB
#define ENC_BTN_Pin GPIO_PIN_14
#define ENC_BTN_GPIO_Port GPIOD
#define ENC_BTN_EXTI_IRQn EXTI15_10_IRQn
#define CH1_ACDC_Pin GPIO_PIN_0
#define CH1_ACDC_GPIO_Port GPIOD
#define CH2_ACDC_Pin GPIO_PIN_1
#define CH2_ACDC_GPIO_Port GPIOD
#define BTN1_Pin GPIO_PIN_5
#define BTN1_GPIO_Port GPIOD
#define BTN1_EXTI_IRQn EXTI9_5_IRQn
#define BTN2_Pin GPIO_PIN_6
#define BTN2_GPIO_Port GPIOD
#define BTN2_EXTI_IRQn EXTI9_5_IRQn
#define BTN3_Pin GPIO_PIN_7
#define BTN3_GPIO_Port GPIOD
#define BTN3_EXTI_IRQn EXTI9_5_IRQn
#define LCD_RST_Pin GPIO_PIN_5
#define LCD_RST_GPIO_Port GPIOB
#define LCD_DC_Pin GPIO_PIN_6
#define LCD_DC_GPIO_Port GPIOB
#define CH1_S0_Pin GPIO_PIN_7
#define CH1_S0_GPIO_Port GPIOB
#define CH1_S1_Pin GPIO_PIN_8
#define CH1_S1_GPIO_Port GPIOB
#define BTN4_Pin GPIO_PIN_9
#define BTN4_GPIO_Port GPIOB
#define BTN4_EXTI_IRQn EXTI9_5_IRQn
#define ADC_PWRDN_Pin GPIO_PIN_0
#define ADC_PWRDN_GPIO_Port GPIOE
#define CH1_MODE_Pin GPIO_PIN_1
#define CH1_MODE_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
