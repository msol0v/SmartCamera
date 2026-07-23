/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os2.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

  typedef enum
  {
    CONTROL_MODE_MVS,
    CONTROL_MODE_PLC
} ControlMode_t;

  typedef enum
  {
    PWM_SOURCE_MVS,
    PWM_SOURCE_PLC
} PWM_Source_t;

  typedef struct
  {
    PWM_Source_t source;
    uint16_t width;
  } PWM_Message_t;

  typedef struct
  {
    uint8_t motor;
    int8_t step;
  } MotorCommand_t;

  extern volatile ControlMode_t controlMode;

  extern osMessageQueueId_t pwmQueueHandle;
  extern osMessageQueueId_t motorQueueHandle;

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
  void startDhcpCheckTask(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RESET_PHY_Pin GPIO_PIN_3
#define RESET_PHY_GPIO_Port GPIOE
#define LED_CNTRL_Pin GPIO_PIN_15
#define LED_CNTRL_GPIO_Port GPIOB
#define IN3_DRIV_Pin GPIO_PIN_10
#define IN3_DRIV_GPIO_Port GPIOC
#define IN2_DRIV_Pin GPIO_PIN_11
#define IN2_DRIV_GPIO_Port GPIOC
#define IN4_DRIV_Pin GPIO_PIN_12
#define IN4_DRIV_GPIO_Port GPIOC
#define IN1_DRIV_Pin GPIO_PIN_0
#define IN1_DRIV_GPIO_Port GPIOD
#define EN_DRIV1_Pin GPIO_PIN_1
#define EN_DRIV1_GPIO_Port GPIOD
#define REED_SW1_Pin GPIO_PIN_2
#define REED_SW1_GPIO_Port GPIOD
#define REED_SW2_Pin GPIO_PIN_3
#define REED_SW2_GPIO_Port GPIOD
#define REED_SW3_Pin GPIO_PIN_4
#define REED_SW3_GPIO_Port GPIOD
#define REED_SW4_Pin GPIO_PIN_5
#define REED_SW4_GPIO_Port GPIOD
#define REED_SW5_Pin GPIO_PIN_6
#define REED_SW5_GPIO_Port GPIOD
#define REED_SW6_Pin GPIO_PIN_7
#define REED_SW6_GPIO_Port GPIOD
#define EN_DRIV2_Pin GPIO_PIN_8
#define EN_DRIV2_GPIO_Port GPIOB
#define EN_DRIV3_Pin GPIO_PIN_9
#define EN_DRIV3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
