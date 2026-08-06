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
    stepper_setSpeed(&stepper, 5);
    uint8_t dir = 0;
    uint8_t gercon_left_idx = 0, gercon_right_idx = 0;
    for(;;)
    {
        if(osMessageQueueGet(motorQueueHandle,&cmd,NULL,osWaitForever) == osOK)
        {
            // Выбор потора переключением EN
            switch (cmd.motor) { // Этот кусок очень тупой, но камень сильный, может себе позволить такое
                case 0:
                    HAL_GPIO_WritePin(EN_DRIV2_GPIO_Port, EN_DRIV2_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(EN_DRIV3_GPIO_Port, EN_DRIV3_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(EN_DRIV1_GPIO_Port, EN_DRIV1_Pin, GPIO_PIN_SET);
                    gercon_left_idx = 0;
                    gercon_right_idx = 1;
                    break;
                case 1:
                    HAL_GPIO_WritePin(EN_DRIV1_GPIO_Port, EN_DRIV1_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(EN_DRIV3_GPIO_Port, EN_DRIV3_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(EN_DRIV2_GPIO_Port, EN_DRIV2_Pin, GPIO_PIN_SET);
                    gercon_left_idx = 2;
                    gercon_right_idx = 3;
                    break;
                case 2:
                    HAL_GPIO_WritePin(EN_DRIV1_GPIO_Port, EN_DRIV1_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(EN_DRIV2_GPIO_Port, EN_DRIV2_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(EN_DRIV3_GPIO_Port, EN_DRIV3_Pin, GPIO_PIN_SET);
                    gercon_left_idx = 4;
                    gercon_right_idx = 5;
                    break;
                default:
                    HAL_GPIO_WritePin(EN_DRIV1_GPIO_Port, EN_DRIV1_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(EN_DRIV2_GPIO_Port, EN_DRIV2_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(EN_DRIV3_GPIO_Port, EN_DRIV3_Pin, GPIO_PIN_RESET);
            }

            // Если зажат левый концевик И поступила команда ехать ВЛЕВО (< 0) -> пропускаем
            if ((bState.stateGercons[gercon_left_idx] == GPIO_PIN_SET) && (cmd.step < 0))
            {
                // Пропускаем команду, двигаться влево дальше нельзя
                continue;
            }

            if ((bState.stateGercons[gercon_right_idx] == GPIO_PIN_SET) && (cmd.step > 0))
            {
                // Пропускаем команду, двигаться вправо дальше нельзя
                continue;
            }

            // Если концевики не блокируют движение в выбранную сторону — выполняем шаги
            stepper_step(&stepper, cmd.step);
        }
    }
}