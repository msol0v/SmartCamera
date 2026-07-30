//
// Created by ms0lov on 7/22/26.
//

#ifndef SMARTCAMERA_CLI_H
#define SMARTCAMERA_CLI_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "string.h"
#include "stdio.h"
#include "FreeRTOS_CLI.h"
//#include "stm32f7xx_hal_uart.h"
#include "stm32f7xx.h"
#include "cmsis_os2.h"


#define MAX_INPUT_LENGTH    50
#define MAX_OUTPUT_LENGTH   1000

#define HISTORY_MAX_DEPTH   10  // Размер буфера истории команд

extern osThreadId_t cliTaskHandle;
extern const osThreadAttr_t cliTask_attributes;

void vCommandConsoleTask( void *pvParameters );

extern void vRegisterCommands(void);

#endif //SMARTCAMERA_CLI_H
