//
// Created by msol0v on 13.04.2026.
//

#ifndef ETHTEST_1_STEPPER_H
#define ETHTEST_1_STEPPER_H

#include "stm32f7xx_hal.h"

typedef struct {
    GPIO_TypeDef *gpio_port;
    uint16_t gpio_pin;
} MotorPin_t;

typedef struct {
    int direction;
    unsigned int step_delay;
    int number_of_steps;
    int step_number;
    unsigned long last_step_time;
    MotorPin_t motor_pin_1;
    MotorPin_t motor_pin_2;
    MotorPin_t motor_pin_3;
    MotorPin_t motor_pin_4;
} Stepper_t;

/* Конструктор для объекта шагового двигателя с 4-пина
 *
 * Рекомендуется использовать макросы HAL библиотеки чтобы не напутать с адресами пинов
 *
 *  number_of_steps - Общее количество шагов двигателя
 *  *motor_pins - Указатель на массив ножек, подключенных к двигателю
 */
Stepper_t stepper_init(int number_of_steps,  MotorPin_t *motor_pins);
void stepper_setSpeed(Stepper_t *motor, long whatSpeed);
void stepper_step(Stepper_t *motor, int steps_to_move);

#endif //ETHTEST_1_STEPPER_H