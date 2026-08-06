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

#define PRESETS_NUM        3
#define PRESET_NAME_LEN    64
#define MOTORS_NUM         3
#define GERCONS_NUM        6

  typedef enum
  {
    CONTROL_MODE_MVS,
    CONTROL_MODE_PLC,
    CONTROL_MODE_WEB
  } ControlMode_t;

  typedef struct
  {
    uint8_t motor;
    int8_t step;
  } MotorCommand_t;

  typedef enum {
    TEST_MOTOR_1 = 0,
    TEST_MOTOR_2 = 1,
    TEST_MOTOR_3 = 2,
    TEST_MOTOR_ALL = 7
  }TestMotorsMask_t;

// Структура состояния системы
typedef struct {
  uint32_t sessionID;
  uint8_t controlMode;        // 0: MVS, 1: PLC, 2: WEB
  int8_t selectedMotor;       // 0: Диафрагма, 1: Резкость, 2: Фокус (-1 если не выбран)
  int8_t lastCmd;             // Последняя выполненная команда
  uint8_t isActiveAutoTest;   // 1 - идет тест, 0 - стоп
  uint8_t testMotorsMask;     // Битовая маска моторов для теста
  uint8_t testCyclesLeft;     // Оставшееся количество циклов
  int8_t currentPreset;       // Текущий пресет (-1 если не активен)

  uint32_t totalStepsMotors[MOTORS_NUM];
  uint32_t avgStepsMotors[MOTORS_NUM];
  uint8_t isMotorsCalibrated[MOTORS_NUM]; // флаги калибровки
  uint8_t stateGercons[GERCONS_NUM];    // Состояние 6 концевиков (0 или 1)

  int32_t presetPositions[PRESETS_NUM][MOTORS_NUM]; // [слот][мотор] (999999 - если пустой)
  char presetNames[MOTORS_NUM][PRESET_NAME_LEN];

  uint8_t ip[4];
  uint8_t netmask[4];
  uint8_t gateway[4];
} BoardState_t;

  extern volatile BoardState_t bState;

  extern volatile ControlMode_t controlMode;

  extern osMessageQueueId_t pwmQueueHandle;
  extern osMessageQueueId_t motorQueueHandle;


  typedef struct {
    uint8_t target_motors_mask; // Битовая маска моторов (bit 0: Диафрагма, bit 1: Резкость, bit 2: Фокус)
    uint16_t total_cycles;      // количество циклов
  } TestTaskArgs_t;

  void AutoTestTask(void *argument);
  extern osThreadId_t autoTestTaskHandle;
  extern const osThreadAttr_t autoTestTask_attributes;
  uint8_t StartAutoTest(uint8_t motors_mask, uint16_t cycles);
  void StopAutoTest(void);
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
#define REED_SW_Port GPIOD
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
