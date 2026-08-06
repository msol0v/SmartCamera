#include "FreeRTOS.h"
#include "main.h"
#include "cmsis_os2.h"
#include <stdlib.h>

osThreadId_t autoTestTaskHandle = NULL;

const osThreadAttr_t autoTestTask_attributes = {
  .name = "AutoTestTask",
  .stack_size = 256 * 4,        // 1024 байта
  .priority = (osPriority_t) osPriorityNormal,
};

void AutoTestTask(void *argument)
{
    // Извлекаем аргументы и освобождаем динамическую память
    TestTaskArgs_t *args = (TestTaskArgs_t *)argument;
    uint8_t motors_mask = args->target_motors_mask;
    uint16_t total_cycles = args->total_cycles;
    vPortFree(args);

    // Инициализируем флаги в глобальной структуре bState
    bState.isActiveAutoTest = 1;
    bState.testMotorsMask = motors_mask;
    bState.testCyclesLeft = total_cycles;

    for (int i = 0; i < 3; i++) {
        bState.totalStepsMotors[i] = 0;
        bState.avgStepsMotors[i] = 0;
    }

    MotorCommand_t cmd; // Переменная команды для отправки в очередь

    // =========================================================================
    // ЭТАП 0: Калибровка (Хоминг(прикольно) к левому концевику)
    // =========================================================================
    for (uint8_t i = 0; i < 3; i++) {
        if (motors_mask & (1 << i)) {
            cmd.motor = i;
            cmd.step = -100; // Движение влево

            // Едем влево (-1), пока не сработает левый геркон (stateGercons[i*2])
            while (bState.stateGercons[i * 2] == 0) {
                // Если очередь переполнена, ждем 1 мс и пробуем снова
                if (osMessageQueuePut(motorQueueHandle, &cmd, 0, 0) != osOK) {
                    osDelay(1);
                    continue;
                }

                // Задержка между шагами (задает скорость движения)
                osDelay(2);
            }

            bState.isMotorsCalibrated[i] = 1;
        }
    }

    // =========================================================================
    // ЦИКЛЫ ТЕСТИРОВАНИЯ
    // =========================================================================
    for (uint16_t cycle = 1; cycle <= total_cycles; cycle++) {

        // --- ЭТАП 1: Движение вправо (+1) до правого концевика ---
        for (uint8_t i = 0; i < 3; i++) {
            if (motors_mask & (1 << i)) {
                uint32_t steps_in_this_run = 0;
                cmd.motor = i;
                cmd.step = 100; // Движение вправо

                // Едем вправо, пока не сработает правый геркон (stateGercons[i*2 + 1])
                while (bState.stateGercons[i * 2 + 1] == 0) {
                    if (osMessageQueuePut(motorQueueHandle, &cmd, 0, 0) == osOK) {
                        steps_in_this_run++;
                        osDelay(2);
                    } else {
                        osDelay(1); // Даем задаче мотора время разгрести очередь
                    }
                }

                bState.totalStepsMotors[i] += steps_in_this_run;
            }
        }

        // --- ЭТАП 2: Движение влево (-1) до левого концевика ---
        for (uint8_t i = 0; i < 3; i++) {
            if (motors_mask & (1 << i)) {
                uint32_t steps_in_this_run = 0;
                cmd.motor = i;
                cmd.step = -100; // Движение влево

                // Едем влево, пока не сработает левый геркон
                while (bState.stateGercons[i * 2] == 0) {
                    if (osMessageQueuePut(motorQueueHandle, &cmd, 0, 0) == osOK) {
                        steps_in_this_run++;
                        osDelay(2);
                    } else {
                        osDelay(1);
                    }
                }

                bState.totalStepsMotors[i] += steps_in_this_run;

                // Расчет среднего количества шагов (2 прогона за цикл)
                bState.avgStepsMotors[i] = bState.totalStepsMotors[i] / (cycle * 2);
            }
        }

        // Обновляем оставшееся количество циклов
        bState.testCyclesLeft = (uint8_t)(total_cycles - cycle);
    }

    // =========================================================================
    // ЗАВЕРШЕНИЕ
    // =========================================================================
    bState.isActiveAutoTest = 0;
    autoTestTaskHandle = NULL;

    osThreadExit();
}

uint8_t StartAutoTest(uint8_t motors_mask, uint16_t cycles)
{
    // Проверяем, не запущен ли уже тест
    if (autoTestTaskHandle != NULL || bState.isActiveAutoTest) {
        return 0; // Занято / Автотест уже выполняется
    }

    // Выделяем память под структуру аргументов из кучи FreeRTOS
    TestTaskArgs_t *args = (TestTaskArgs_t *)pvPortMalloc(sizeof(TestTaskArgs_t));
    if (args == NULL) {
        return 0; // Ошибка: не хватило RAM
    }

    // Заполняем аргументы
    args->target_motors_mask = motors_mask;
    args->total_cycles = cycles;

    // Динамически создаем задачу
    autoTestTaskHandle = osThreadNew(AutoTestTask, (void *)args, &autoTestTask_attributes);

    // Проверяем, создался ли поток
    if (autoTestTaskHandle == NULL) {
        vPortFree(args); // Если создать поток не удалось, освобождаем память!
        return 0;
    }

    return 1; // Успешно запущен
}

void StopAutoTest(void)
{
    // Принудительно уничтожаем задачу автотеста
    if (autoTestTaskHandle != NULL) {
        osThreadTerminate(autoTestTaskHandle);
        autoTestTaskHandle = NULL;
    }

    // Сбрасываем очередь, чтобы моторы не доделывали "повисшие" шаги
    if (motorQueueHandle != NULL) {
        osMessageQueueReset(motorQueueHandle);
    }

    // Обновляем статус системы
    bState.isActiveAutoTest = 0;
    bState.testCyclesLeft = 0;
}