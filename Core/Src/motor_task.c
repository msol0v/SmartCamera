//
// Created by msol0v on 23.07.2026.
//

#include "motor_task.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

#include "stepper.h"

osThreadId_t motorTaskHandle;

const osThreadAttr_t motorTask_attributes = {
    .name = "pwmTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal,
  };

// Stepper STEPPERS[3] = {
//     Stepper(STEPS_PER_REVOLUTION, 34, 36, 35, 37),
//     Stepper(STEPS_PER_REVOLUTION, 43, 45, 44, 46),
//     Stepper(STEPS_PER_REVOLUTION, 13, 11, 12, 10)
//   };
#define STEPS_PER_REVOLUTION 5960
void MotorTask(void *argument)
{
    MotorCommand_t cmd;
    MotorPin_t pins[4] = {
        {.gpio_port = IN1_DRIV_GPIO_Port,.gpio_pin = IN1_DRIV_Pin},
        {.gpio_port = IN3_DRIV_GPIO_Port, .gpio_pin = IN3_DRIV_Pin},
        {.gpio_port = IN2_DRIV_GPIO_Port, .gpio_pin = IN2_DRIV_Pin},
        {.gpio_port = IN4_DRIV_GPIO_Port, .gpio_pin = IN4_DRIV_Pin}
    };
    Stepper_t stepper = stepper_init(STEPS_PER_REVOLUTION, pins);
    stepper_setSpeed(&stepper, 1);
    HAL_GPIO_WritePin(EN_DRIV1_GPIO_Port, EN_DRIV1_Pin, GPIO_PIN_SET);
    uint8_t dir = 0;
    for(;;)
    {
        if(osMessageQueueGet(motorQueueHandle,&cmd,NULL,osWaitForever) == osOK)
        {
            // Если зажат левый концевик И поступила команда ехать ВЛЕВО (< 0) -> пропускаем
            if ((bState.stateGercons[4] == GPIO_PIN_SET) && (cmd.step < 0))
            {
                // Пропускаем команду, двигаться влево дальше нельзя
                continue;
            }

            if ((bState.stateGercons[2] == GPIO_PIN_SET) && (cmd.step > 0))
            {
                // Пропускаем команду, двигаться вправо дальше нельзя
                continue;
            }

            // 3. Если концевики не блокируют движение в выбранную сторону — выполняем шаги
            stepper_step(&stepper, cmd.step);
        }
    }
}