//
// Created by msol0v on 23.07.2026.
//

#ifndef SMARTCAMERA_PWM_TASK_H
#define SMARTCAMERA_PWM_TASK_H

#include "FreeRTOS.h"

#include "queue.h"
#include "cmsis_os2.h"
#include "main.h"

typedef struct {
    uint8_t source;
    uint32_t width;   // Длительность импульса (CCR2)
    uint32_t period;  // Полный период (CCR1), если требуется (потом можно убрать)
} PWM_Message_t;

extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim9;
extern osThreadId_t pwmTaskHandle;
extern const osThreadAttr_t pwmTask_attributes;
extern osMessageQueueId_t pwmQueueHandle;

/* Функции запуска-остановки работы таймеров
 * Задачу останавливать не нужно, она спит пока очередь пустая - таймер спит - в очередь не пишет
 * Дополнительно функции остановки чтения чистят очередь
 */
void stopCurrentModeRead(void);
void startCurrendModeRead(void);

// Функция таски обработки ШИМ
void PWM_Task(void *argument);

#endif //SMARTCAMERA_PWM_TASK_H