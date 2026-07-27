//
// Created by msol0v on 23.07.2026.
//

#ifndef SMARTCAMERA_MOTOR_TASK_H
#define SMARTCAMERA_MOTOR_TASK_H

#include "cmsis_os2.h"

void MotorTask(void *argument);

extern osThreadId_t motorTaskHandle;
extern const osThreadAttr_t motorTask_attributes;


#endif //SMARTCAMERA_MOTOR_TASK_H