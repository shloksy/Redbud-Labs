/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32h7xx_hal.h"

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
#define LCD_DISP_Pin GPIO_PIN_8
#define LCD_DISP_GPIO_Port GPIOI
#define SD_CD_Pin GPIO_PIN_13
#define SD_CD_GPIO_Port GPIOC
#define HSC_CS_Pin GPIO_PIN_11
#define HSC_CS_GPIO_Port GPIOI
#define TP_RESET_Pin GPIO_PIN_6
#define TP_RESET_GPIO_Port GPIOF
#define OPTICAL_LED_3_Pin GPIO_PIN_7
#define OPTICAL_LED_3_GPIO_Port GPIOF
#define VALVE_EXPANDER4_CS_Pin GPIO_PIN_9
#define VALVE_EXPANDER4_CS_GPIO_Port GPIOF
#define OPTICAL_FEEDBACK3_Pin GPIO_PIN_1
#define OPTICAL_FEEDBACK3_GPIO_Port GPIOC
#define PUSHBUTTON_SWITCH_Pin GPIO_PIN_2
#define PUSHBUTTON_SWITCH_GPIO_Port GPIOC
#define OPTICAL_LED_1_Pin GPIO_PIN_0
#define OPTICAL_LED_1_GPIO_Port GPIOA
#define SMC_nEN3_Pin GPIO_PIN_1
#define SMC_nEN3_GPIO_Port GPIOA
#define OPTICAL_LED_2_Pin GPIO_PIN_2
#define OPTICAL_LED_2_GPIO_Port GPIOA
#define DEBUG_LED_Pin GPIO_PIN_4
#define DEBUG_LED_GPIO_Port GPIOH
#define RPI_PWR_EN_Pin GPIO_PIN_4
#define RPI_PWR_EN_GPIO_Port GPIOA
#define SMC_nEN2_Pin GPIO_PIN_4
#define SMC_nEN2_GPIO_Port GPIOC
#define BLDC_EN_Pin GPIO_PIN_5
#define BLDC_EN_GPIO_Port GPIOC
#define BLDC_IN2_Pin GPIO_PIN_2
#define BLDC_IN2_GPIO_Port GPIOB
#define MOTION_FB_C_Pin GPIO_PIN_10
#define MOTION_FB_C_GPIO_Port GPIOB
#define BLDC_IN1_Pin GPIO_PIN_11
#define BLDC_IN1_GPIO_Port GPIOB
#define SP_RESET_Pin GPIO_PIN_11
#define SP_RESET_GPIO_Port GPIOH
#define PUMP_FEEDBACK_Pin GPIO_PIN_12
#define PUMP_FEEDBACK_GPIO_Port GPIOH
#define VALVE_EXPANDER1_CS_Pin GPIO_PIN_12
#define VALVE_EXPANDER1_CS_GPIO_Port GPIOB
#define REGULATOR_CS_Pin GPIO_PIN_11
#define REGULATOR_CS_GPIO_Port GPIOD
#define ROCKER_SWITCH1_Pin GPIO_PIN_12
#define ROCKER_SWITCH1_GPIO_Port GPIOD
#define FAN_SPEED_FEEDBACK_Pin GPIO_PIN_13
#define FAN_SPEED_FEEDBACK_GPIO_Port GPIOD
#define PUMP_CS_Pin GPIO_PIN_2
#define PUMP_CS_GPIO_Port GPIOG
#define VALVE_EXPANDER2_CS_Pin GPIO_PIN_3
#define VALVE_EXPANDER2_CS_GPIO_Port GPIOG
#define MOTION_HALL_FEEDBACK_Pin GPIO_PIN_6
#define MOTION_HALL_FEEDBACK_GPIO_Port GPIOC
#define SMC_nEN1_Pin GPIO_PIN_7
#define SMC_nEN1_GPIO_Port GPIOC
#define AD7172_GPIO0_Pin GPIO_PIN_8
#define AD7172_GPIO0_GPIO_Port GPIOC
#define HEATER1_Pin GPIO_PIN_9
#define HEATER1_GPIO_Port GPIOC
#define HEATER2_Pin GPIO_PIN_8
#define HEATER2_GPIO_Port GPIOA
#define RPI_PWR_nFLG_Pin GPIO_PIN_14
#define RPI_PWR_nFLG_GPIO_Port GPIOH
#define TMC429_CS_Pin GPIO_PIN_3
#define TMC429_CS_GPIO_Port GPIOI
#define HEATER1_ALT_Pin GPIO_PIN_15
#define HEATER1_ALT_GPIO_Port GPIOA
#define AD7172_CS_Pin GPIO_PIN_12
#define AD7172_CS_GPIO_Port GPIOC
#define THERMAL_nCS3_Pin GPIO_PIN_2
#define THERMAL_nCS3_GPIO_Port GPIOD
#define BLDC_DIR_Pin GPIO_PIN_3
#define BLDC_DIR_GPIO_Port GPIOD
#define PUMP_ON_Pin GPIO_PIN_4
#define PUMP_ON_GPIO_Port GPIOD
#define VALVE_EXPANDER3_CS_Pin GPIO_PIN_5
#define VALVE_EXPANDER3_CS_GPIO_Port GPIOD
#define OPTICAL_FEEDBACK4_Pin GPIO_PIN_7
#define OPTICAL_FEEDBACK4_GPIO_Port GPIOD
#define THERMAL_nCS2_Pin GPIO_PIN_9
#define THERMAL_nCS2_GPIO_Port GPIOG
#define OPTICAL_FEEDBACK2_Pin GPIO_PIN_13
#define OPTICAL_FEEDBACK2_GPIO_Port GPIOG
#define OPTICAL_FEEDBACK1_Pin GPIO_PIN_14
#define OPTICAL_FEEDBACK1_GPIO_Port GPIOG
#define MOTION_FB_B_Pin GPIO_PIN_4
#define MOTION_FB_B_GPIO_Port GPIOB
#define BLDC_STATUS_Pin GPIO_PIN_5
#define BLDC_STATUS_GPIO_Port GPIOB
#define SOM_CMD_PWR_OFF_Pin GPIO_PIN_6
#define SOM_CMD_PWR_OFF_GPIO_Port GPIOB
#define MOTION_FB_A_Pin GPIO_PIN_7
#define MOTION_FB_A_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define BSP_LED_PIN		DEBUG_LED_Pin
#define BSP_LED_PORT	DEBUG_LED_GPIO_Port
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
