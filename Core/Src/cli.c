
#include "cli.h"

extern UART_HandleTypeDef huart7;

osThreadId_t cliTaskHandle;

const osThreadAttr_t cliTask_attributes = {
    .name = "cliTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal,
  };

// Очередь для передачи символов из ISR в таску
QueueHandle_t xRxQueue = NULL;
static uint8_t u8RxChar; // Буфер для приема 1 байта по IT

// Запуск приема в режиме прерываний
void Console_Init(void)
{
    xRxQueue = xQueueCreate(128, sizeof(char));
    // Начинаем слушать UART7 по 1 байту
    HAL_UART_Receive_IT(&huart7, &u8RxChar, 1);
}

// Колбэк приема
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART7)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        // Отправляем принятый символ в очередь из прерывания
        xQueueSendFromISR(xRxQueue, &u8RxChar, &xHigherPriorityTaskWoken);

        // Перезапускаем прием следующего байта
        HAL_UART_Receive_IT(&huart7, &u8RxChar, 1);

        // Разбудить таску немедленно, если она ждет в очереди и имеет более высокий приоритет
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void vCommandConsoleTask(void *pvParameters)
{
    UART_HandleTypeDef *pxUart = (UART_HandleTypeDef *)pvParameters;
    char cRxedChar;
    int8_t cInputIndex = 0;
    BaseType_t xMoreDataToFollow;

    static char pcOutputString[MAX_OUTPUT_LENGTH];
    static char pcInputString[MAX_INPUT_LENGTH];

    vRegisterIPCommand();

    // Инициализируем прием UART и очередь
    Console_Init();

    // Приветственное сообщение
    printf("\r\n--- CLI Ready ---\r\n>");

    for (;;)
    {
        if (xQueueReceive(xRxQueue, &cRxedChar, portMAX_DELAY) == pdPASS)
        {
            // Обработка Enter (\n или \r)
            if (cRxedChar == '\n' || cRxedChar == '\r')
            {
                if (cInputIndex > 0) // Обрабатываем только если команда не пустая
                {
                    HAL_UART_Transmit(pxUart, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);

                    // Цикл обработки CLI команды
                    do
                    {
                        xMoreDataToFollow = FreeRTOS_CLIProcessCommand(
                            pcInputString,
                            pcOutputString,
                            MAX_OUTPUT_LENGTH
                        );

                        // Отправка ответа в UART7
                        HAL_UART_Transmit(pxUart, (uint8_t *)pcOutputString, strlen(pcOutputString), HAL_MAX_DELAY);

                    } while (xMoreDataToFollow != pdFALSE);

                    // Сброс буфера ввода
                    cInputIndex = 0;
                    memset(pcInputString, 0x00, MAX_INPUT_LENGTH);

                    // Выводим приглашение ввода
                    HAL_UART_Transmit(pxUart, (uint8_t *)"\r\n>", 3, HAL_MAX_DELAY);
                }
            }
            else
            {
                // Обработка Backspace (ASCII 0x08 или 0x7F)
                if (cRxedChar == '\b' || cRxedChar == 0x7F)
                {
                    if (cInputIndex > 0)
                    {
                        cInputIndex--;
                        pcInputString[cInputIndex] = '\0';

                        // Эхо-эффект стирания символа в терминале (назад, пробел, назад)
                        HAL_UART_Transmit(pxUart, (uint8_t *)"\b \b", 3, HAL_MAX_DELAY);
                    }
                }
                else // Обычный символ
                {
                    if (cInputIndex < (MAX_INPUT_LENGTH - 1))
                    {
                        pcInputString[cInputIndex] = cRxedChar;
                        cInputIndex++;
                        pcInputString[cInputIndex] = '\0'; // Гарантируем null-terminated строку

                        // Эхо-печать введенного символа обратно в консоль
                        HAL_UART_Transmit(pxUart, (uint8_t *)&cRxedChar, 1, HAL_MAX_DELAY);
                    }
                }
            }
        }
    }
}