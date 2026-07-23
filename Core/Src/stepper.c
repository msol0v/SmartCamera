//
// Created by msol0v on 13.04.2026.
//

#include "stepper.h"

#include <stdlib.h>

#include "cmsis_os2.h"

Stepper_t stepper_init(int number_of_steps,  MotorPin_t *motor_pins) {
    Stepper_t stepper = {
        .direction = 0,
        .step_number = 0,
        .last_step_time = 0,
        .number_of_steps = number_of_steps,
        .motor_pin_1 = motor_pins[0],
        .motor_pin_2 = motor_pins[1],
        .motor_pin_3 = motor_pins[2],
        .motor_pin_4 = motor_pins[3],
    };
    return stepper;
}

/*
 * Устанавливает скорость в оборотах в минуту
 *
 * !!! В отличие от оригинальной библиотеки, где время считается в мкс, тут используется мс
 */
void stepper_setSpeed(Stepper_t *motor, long whatSpeed) {
    motor->step_delay = 60L * 1000L / motor->number_of_steps / whatSpeed;
}

static void stepMotor(Stepper_t *motor, int thisStep) {
    switch (thisStep) {
        case 0:  // 1010
            HAL_GPIO_WritePin(motor->motor_pin_1.gpio_port, motor->motor_pin_1.gpio_pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(motor->motor_pin_2.gpio_port, motor->motor_pin_2.gpio_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(motor->motor_pin_3.gpio_port, motor->motor_pin_3.gpio_pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(motor->motor_pin_4.gpio_port, motor->motor_pin_4.gpio_pin, GPIO_PIN_RESET);
            break;
        case 1:  // 0110
            HAL_GPIO_WritePin(motor->motor_pin_1.gpio_port, motor->motor_pin_1.gpio_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(motor->motor_pin_2.gpio_port, motor->motor_pin_2.gpio_pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(motor->motor_pin_3.gpio_port, motor->motor_pin_3.gpio_pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(motor->motor_pin_4.gpio_port, motor->motor_pin_4.gpio_pin, GPIO_PIN_RESET);
            break;
        case 2:  //0101
            HAL_GPIO_WritePin(motor->motor_pin_1.gpio_port, motor->motor_pin_1.gpio_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(motor->motor_pin_2.gpio_port, motor->motor_pin_2.gpio_pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(motor->motor_pin_3.gpio_port, motor->motor_pin_3.gpio_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(motor->motor_pin_4.gpio_port, motor->motor_pin_4.gpio_pin, GPIO_PIN_SET);
            break;
        case 3:  //1001
            HAL_GPIO_WritePin(motor->motor_pin_1.gpio_port, motor->motor_pin_1.gpio_pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(motor->motor_pin_2.gpio_port, motor->motor_pin_2.gpio_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(motor->motor_pin_3.gpio_port, motor->motor_pin_3.gpio_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(motor->motor_pin_4.gpio_port, motor->motor_pin_4.gpio_pin, GPIO_PIN_SET);
            break;
    }
}

void stepper_step(Stepper_t *motor, int steps_to_move) {
    int steps_left = abs(steps_to_move);    // Общее количество шагов без направления
    if (steps_to_move > 0) { motor->direction = 1; }    // Направление
    if (steps_to_move < 0) { motor->direction = 0; }

    // Пока все шаги не сделаем
    while (steps_left > 0) {
        // 1 тик = 1 мс
        uint32_t now = osKernelGetTickCount();

        if (now - motor->last_step_time >= motor->step_delay) {
            motor->last_step_time = now;

            if (motor->direction == 1) {
                motor->step_number++;
                if (motor->step_number == motor->number_of_steps)
                    motor->step_number = 0;
            }
            else {
                if (motor->step_number ==0)
                    motor->step_number = motor->number_of_steps;
                motor->step_number--;
            }
            steps_left--;

            stepMotor(motor, motor->step_number % 4);
        }
    }
}

