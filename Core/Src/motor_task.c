//
// Created by msol0v on 23.07.2026.
//

#include "motor_task.h"
#include "main.h"
#include <stdio.h>

void MotorTask(void *argument)
{
    MotorCommand_t cmd;

    for(;;)
    {
        if(osMessageQueueGet(motorQueueHandle,&cmd,NULL,osWaitForever) == osOK)
        {
            printf("Motor=%d Step=%d\r\n", cmd.motor, cmd.step);

            // MotorMove(cmd.motor, cmd.step);
        }
    }
}