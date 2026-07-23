//
// Created by msol0v on 23.07.2026.
//

#ifndef SMARTCAMERA_PWM_TASK_H
#define SMARTCAMERA_PWM_TASK_H

#include "cmsis_os.h"
#include "queue.h"
#include "cmsis_os2.h"
#include "main.h"


void PWM_Task(void *argument);
void changePWMsrc(PWM_Source_t source);

extern osThreadId_t pwmTaskHandle;
extern const osThreadAttr_t pwmTask_attributes;

#endif //SMARTCAMERA_PWM_TASK_H