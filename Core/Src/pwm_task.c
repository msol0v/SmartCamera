#include "pwm_task.h"
#include "main.h"
#include <stdio.h>
#include <stdbool.h>

osThreadId_t pwmTaskHandle;

const osThreadAttr_t pwmTask_attributes = {
    .name = "pwmTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal,
  };

// ==================== MVS (1100 - 4300 мкс, шаг 100 мкс) ====================
#define MVS_MIN_US       1100
#define MVS_MAX_US       4300
#define MVS_STEP_US      100
#define MVS_TOLERANCE_US 35     //Погрешность (Из-за оптопары там постоянные +40, с подтяжкой ноги к земле +35)

static const int8_t MVS_STEP_LUT[] = {
    // Двигатель 0: Диафрагма (1100 - 2100 мкс)
    -16, -8, -4, -2, -1, 0, 1, 2, 4, 8, 16,
    // Двигатель 1: Резкость (2200 - 3200 мкс)
    -16, -8, -4, -2, -1, 0, 1, 2, 4, 8, 16,
    // Двигатель 2: Фокус (3300 - 4300 мкс)
    -16, -8, -4, -2, -1, 0, 1, 2, 4, 8, 16
};

// ==================== PLC (10 000 - 140 000 мкс, шаг 10 000 мкс) ====================
#define PLC_MIN_US       10000
#define PLC_MAX_US       140000
#define PLC_STEP_US      10000
#define PLC_TOLERANCE_US 2000 // Погрешность 2 мс

static const int8_t PLC_STEP_LUT[] = {
    -16, -8, -4, -2, -1, 0, 1, 2, 4, 8, 16
};

#define REQUIRED_CONSECUTIVE_COUNT 5
#define INVALID_INDEX              -1

// === Состояние фильтра MVS ===
static int32_t g_mvs_last_index = INVALID_INDEX;
static uint8_t g_mvs_consecutive = 0;
static int32_t g_mvs_last_executed_index = INVALID_INDEX;

// === Состояние фильтра ПЛК ===
static int32_t g_plc_last_index = INVALID_INDEX;
static uint8_t g_plc_consecutive = 0;

// Хранение последней выполненной комбинации в режиме ПЛК
static int8_t  g_plc_last_executed_motor = -1;
static int8_t  g_plc_last_executed_step  = 99; // Заведомо невалидное значение

static void resetPwmFilter(void) {
    g_mvs_last_index = INVALID_INDEX;
    g_mvs_consecutive = 0;
    g_mvs_last_executed_index = INVALID_INDEX;

    g_plc_last_index = INVALID_INDEX;
    g_plc_consecutive = 0;
    g_plc_last_executed_motor = -1;
    g_plc_last_executed_step = 99;
}

void startCurrendModeRead(void) {
    resetPwmFilter();
    if (bState.controlMode == CONTROL_MODE_MVS) {
        HAL_TIM_IC_Start_IT(&htim8, TIM_CHANNEL_1);
        HAL_TIM_IC_Start_IT(&htim8, TIM_CHANNEL_2);
    }
    else if (bState.controlMode == CONTROL_MODE_PLC) {
        HAL_TIM_IC_Start_IT(&htim9, TIM_CHANNEL_1);
        HAL_TIM_IC_Start_IT(&htim9, TIM_CHANNEL_2);
    }
    // Если выбран веб то просто выходим
}

void stopCurrentModeRead(void) {
    if (bState.controlMode == CONTROL_MODE_MVS) {
        HAL_TIM_IC_Stop_IT(&htim8, TIM_CHANNEL_1);
        HAL_TIM_IC_Stop_IT(&htim8, TIM_CHANNEL_2);
    }
    else if (bState.controlMode == CONTROL_MODE_PLC) {
        HAL_TIM_IC_Stop_IT(&htim9, TIM_CHANNEL_1);
        HAL_TIM_IC_Stop_IT(&htim9, TIM_CHANNEL_2);
    }
    osMessageQueueReset(pwmQueueHandle);
    resetPwmFilter();
    // Если веб то и останавливать нечего
}


static int32_t parseMvsCommand(uint32_t pwm_width, MotorCommand_t *cmd) {
    uint32_t rounded_width = ((pwm_width + (MVS_STEP_US / 2)) / MVS_STEP_US) * MVS_STEP_US;

    if (rounded_width < MVS_MIN_US || rounded_width > MVS_MAX_US) return INVALID_INDEX;

    uint32_t diff = (pwm_width > rounded_width) ? (pwm_width - rounded_width) : (rounded_width - pwm_width);
    if (diff > MVS_TOLERANCE_US) return INVALID_INDEX;

    uint32_t index = (rounded_width - MVS_MIN_US) / MVS_STEP_US;
    cmd->motor = (index / 11);
    cmd->step  = MVS_STEP_LUT[index];

    return (int32_t)index;
}

static int32_t parsePlcCommand(uint32_t pwm_width) {
    uint32_t rounded_width = ((pwm_width + (PLC_STEP_US / 2)) / PLC_STEP_US) * PLC_STEP_US;

    if (rounded_width < PLC_MIN_US || rounded_width > PLC_MAX_US) return INVALID_INDEX;

    uint32_t diff = (pwm_width > rounded_width) ? (pwm_width - rounded_width) : (rounded_width - pwm_width);
    if (diff > PLC_TOLERANCE_US) return INVALID_INDEX;

    return (int32_t)((rounded_width - PLC_MIN_US) / PLC_STEP_US);
}

void PWM_Task(void *argument)
{
    PWM_Message_t msg;

    // Запускаем таймер в зависимости от режима работы. Должен быть проинициализирован при старте системы
    startCurrendModeRead();

    for(;;)
    {
        if (osMessageQueueGet(pwmQueueHandle, &msg, NULL, osWaitForever) == osOK)
        {
            // ==================== РЕЖИМ MVS ====================
            if (bState.controlMode == CONTROL_MODE_MVS)
            {
                MotorCommand_t cmd;
                int32_t current_index = parseMvsCommand(msg.width, &cmd);

                if (current_index == INVALID_INDEX) {
                    g_mvs_last_index = INVALID_INDEX;
                    g_mvs_consecutive = 0;
                    continue;
                }

                if (current_index == g_mvs_last_index) {
                    if (g_mvs_consecutive < 255) g_mvs_consecutive++;
                } else {
                    g_mvs_last_index = current_index;
                    g_mvs_consecutive = 1;
                }

                if (g_mvs_consecutive >= REQUIRED_CONSECUTIVE_COUNT) {
                    if (current_index != g_mvs_last_executed_index) {
                        if (osMessageQueuePut(motorQueueHandle, &cmd, 0U, 0U) == osOK) {
                            g_mvs_last_executed_index = current_index;
                        }
                    }
                }
            }
            // ==================== РЕЖИМ ПЛК (PLC) ====================
            else if (bState.controlMode == CONTROL_MODE_PLC)
            {
                int32_t raw_index = parsePlcCommand(msg.width);

                if (raw_index == INVALID_INDEX) {
                    g_plc_last_index = INVALID_INDEX;
                    g_plc_consecutive = 0;
                    continue;
                }

                // 1. Проверка 5 совпадений подряд
                if (raw_index == g_plc_last_index) {
                    if (g_plc_consecutive < 255) g_plc_consecutive++;
                } else {
                    g_plc_last_index = raw_index;
                    g_plc_consecutive = 1;
                }

                // 2. Набрали 5 подтверждений подряд
                if (g_plc_consecutive == REQUIRED_CONSECUTIVE_COUNT)
                {
                    // А) Пришла команда выбора мотора (120 000 — 140 000 мкс)
                    if (raw_index >= 11 && raw_index <= 13)
                    {
                        // 120 мс -> 0 (Диафрагма), 130 мс -> 1 (Резкость), 140 мс -> 2 (Фокус)
                        bState.selectedMotor = (int8_t)(raw_index - 11);
                    }
                    // Б) Пришла команда шага (10 000 — 110 000 мкс)
                    else if (raw_index >= 0 && raw_index <= 10)
                    {
                        // Выполняем шаг только если мотор уже выбран в bState
                        if (bState.selectedMotor >= 0 && bState.selectedMotor <= 2)
                        {
                            int8_t step = PLC_STEP_LUT[raw_index];

                            // Проверка на однократность: не выполняли ли мы только что этот же шаг на этом же моторе?
                            if (bState.selectedMotor != g_plc_last_executed_motor ||
                                step != g_plc_last_executed_step)
                            {
                                MotorCommand_t cmd;
                                cmd.motor = (uint8_t)bState.selectedMotor;
                                cmd.step  = step;

                                if (osMessageQueuePut(motorQueueHandle, &cmd, 0U, 0U) == osOK) {
                                    g_plc_last_executed_motor = bState.selectedMotor;
                                    g_plc_last_executed_step  = step;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// Прерывание захвата таймера
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    // Отслеживаем событие по Каналу 1 (когда сбросился счетчик и захватился период/ширина)
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        PWM_Message_t msg;

        if (htim->Instance == TIM8)
        {
            msg.source = 0;
            msg.width  = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2); // CCR2 - Ширина
            msg.period = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // CCR1 - Период

            // Отправляем в очередь без ожидания (0 ticks)
            osMessageQueuePut(pwmQueueHandle, &msg, 0U, 0U);
        }
        else if (htim->Instance == TIM9)
        {
            msg.source = 1;
            msg.width  = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
            msg.period = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

            osMessageQueuePut(pwmQueueHandle, &msg, 0U, 0U);
        }
    }
}