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
static uint8_t u8RxChar;

// Структура кольцевого буфера истории
static char cliHistory[HISTORY_MAX_DEPTH][MAX_INPUT_LENGTH];
static int historyCount = 0;
static int historyHead = 0;
static int historyIndex = -1;

void Console_Init(void)
{
    xRxQueue = xQueueCreate(128, sizeof(char));
    HAL_UART_Receive_IT(&huart7, &u8RxChar, 1);
}

// Колбек прерывания срабатывает по приему символа
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART7)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(xRxQueue, &u8RxChar, &xHigherPriorityTaskWoken);
        HAL_UART_Receive_IT(&huart7, &u8RxChar, 1);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

// Добавление команды в историю команд
static void pvHistoryAdd(const char *cmd)
{
    if (cmd == NULL || strlen(cmd) == 0) return;

    int lastIdx = (historyHead - 1 + HISTORY_MAX_DEPTH) % HISTORY_MAX_DEPTH;
    if (historyCount > 0 && strcmp(cliHistory[lastIdx], cmd) == 0) {
        return;
    }

    strncpy(cliHistory[historyHead], cmd, MAX_INPUT_LENGTH - 1);
    cliHistory[historyHead][MAX_INPUT_LENGTH - 1] = '\0';

    historyHead = (historyHead + 1) % HISTORY_MAX_DEPTH;
    if (historyCount < HISTORY_MAX_DEPTH) {
        historyCount++;
    }
}

// Стирание введенной строки
static void pvClearCurrentLine(UART_HandleTypeDef *pxUart, char *buf, int8_t *len)
{
    while (*len > 0) {
        HAL_UART_Transmit(pxUart, (uint8_t *)"\b \b", 3, HAL_MAX_DELAY);
        (*len)--;
    }
    *len = 0;
    buf[0] = '\0';
}

void vCommandConsoleTask(void *pvParameters)
{
    UART_HandleTypeDef *pxUart = (UART_HandleTypeDef *)pvParameters;
    char cRxedChar;
    int8_t cInputIndex = 0;
    BaseType_t xMoreDataToFollow;
    static char cLastCRLF = 0;

    static char pcOutputString[MAX_OUTPUT_LENGTH];
    static char pcInputString[MAX_INPUT_LENGTH];
    static char pcCmdCopy[MAX_INPUT_LENGTH]; // Буфер-копия для передаче в FreeRTOS CLI

    uint8_t escState = 0;

    vRegisterCommands();
    Console_Init();

    printf("\r\n--- CLI Ready ---\r\n>");

    for (;;)
    {
        if (xQueueReceive(xRxQueue, &cRxedChar, portMAX_DELAY) == pdPASS)
        {
            // Сбрасываем фильтр парных CRLF для любого символа, кроме \r и \n
            if (cRxedChar != '\r' && cRxedChar != '\n')
            {
                cLastCRLF = 0;
            }

            // === Обработка ANSI Escape-последовательностей (Стрелки) ===
            if (escState == 0)
            {
                if (cRxedChar == 0x1B) {
                    escState = 1;
                    continue;
                }
            }
            else if (escState == 1)
            {
                if (cRxedChar == '[') {
                    escState = 2;
                    continue;
                } else {
                    escState = 0;
                }
            }
            else if (escState == 2)
            {
                escState = 0;

                // Стрелка ВВЕРХ
                if (cRxedChar == 'A')
                {
                    if (historyCount > 0)
                    {
                        if (historyIndex == -1) {
                            historyIndex = (historyHead - 1 + HISTORY_MAX_DEPTH) % HISTORY_MAX_DEPTH;
                        } else {
                            int oldestIdx = (historyHead - historyCount + HISTORY_MAX_DEPTH) % HISTORY_MAX_DEPTH;
                            if (historyIndex != oldestIdx) {
                                historyIndex = (historyIndex - 1 + HISTORY_MAX_DEPTH) % HISTORY_MAX_DEPTH;
                            }
                        }

                        pvClearCurrentLine(pxUart, pcInputString, &cInputIndex);
                        strncpy(pcInputString, cliHistory[historyIndex], MAX_INPUT_LENGTH - 1);
                        pcInputString[MAX_INPUT_LENGTH - 1] = '\0';
                        cInputIndex = (int8_t)strlen(pcInputString);
                        HAL_UART_Transmit(pxUart, (uint8_t *)pcInputString, cInputIndex, HAL_MAX_DELAY);
                    }
                    continue;
                }
                // Стрелка ВНИЗ
                else if (cRxedChar == 'B')
                {
                    if (historyIndex != -1)
                    {
                        int lastIdx = (historyHead - 1 + HISTORY_MAX_DEPTH) % HISTORY_MAX_DEPTH;
                        if (historyIndex == lastIdx) {
                            historyIndex = -1;
                            pvClearCurrentLine(pxUart, pcInputString, &cInputIndex);
                        } else {
                            historyIndex = (historyIndex + 1) % HISTORY_MAX_DEPTH;
                            pvClearCurrentLine(pxUart, pcInputString, &cInputIndex);
                            strncpy(pcInputString, cliHistory[historyIndex], MAX_INPUT_LENGTH - 1);
                            pcInputString[MAX_INPUT_LENGTH - 1] = '\0';
                            cInputIndex = (int8_t)strlen(pcInputString);
                            HAL_UART_Transmit(pxUart, (uint8_t *)pcInputString, cInputIndex, HAL_MAX_DELAY);
                        }
                    }
                    continue;
                }
            }

            // === Обработка Ctrl + C ===
            if (cRxedChar == 0x03)
            {
                HAL_UART_Transmit(pxUart, (uint8_t *)"^C\r\n>", 5, HAL_MAX_DELAY);
                cInputIndex = 0;
                pcInputString[0] = '\0';
                historyIndex = -1;
                continue;
            }

            // === Обработка Enter (\r или \n) ===
            if (cRxedChar == '\n' || cRxedChar == '\r')
            {
                // Фильтрация дублирующегося \n после \r (или наоборот)
                if (cLastCRLF != 0 && cLastCRLF != cRxedChar)
                {
                    cLastCRLF = 0;
                    continue;
                }
                cLastCRLF = cRxedChar;

                if (cInputIndex > 0)
                {
                    HAL_UART_Transmit(pxUart, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);

                    // 1. Сохраняем команду в историю
                    pvHistoryAdd(pcInputString);
                    historyIndex = -1; // Сбрасываем индекс навигации по истории

                    // 2. Делаем КОПИЮ строки, т.к. FreeRTOS_CLIProcessCommand мутирует буфер!
                    strncpy(pcCmdCopy, pcInputString, MAX_INPUT_LENGTH - 1);
                    pcCmdCopy[MAX_INPUT_LENGTH - 1] = '\0';

                    // 3. Выполняем команду по копии
                    do
                    {
                        xMoreDataToFollow = FreeRTOS_CLIProcessCommand(
                            pcCmdCopy,
                            pcOutputString,
                            MAX_OUTPUT_LENGTH
                        );

                        HAL_UART_Transmit(pxUart, (uint8_t *)pcOutputString, strlen(pcOutputString), HAL_MAX_DELAY);

                    } while (xMoreDataToFollow != pdFALSE);

                    // 4. Гарантированный сброс буфера ввода
                    cInputIndex = 0;
                    memset(pcInputString, 0, MAX_INPUT_LENGTH);

                    // Приглашение
                    HAL_UART_Transmit(pxUart, (uint8_t *)"\r\n>", 3, HAL_MAX_DELAY);
                }
                else
                {
                    // Enter на пустой строке
                    historyIndex = -1;
                    HAL_UART_Transmit(pxUart, (uint8_t *)"\r\n>", 3, HAL_MAX_DELAY);
                }
            }
            // === Обработка Backspace ===
            else if (cRxedChar == '\b' || cRxedChar == 0x7F)
            {
                if (cInputIndex > 0)
                {
                    cInputIndex--;
                    pcInputString[cInputIndex] = '\0';
                    HAL_UART_Transmit(pxUart, (uint8_t *)"\b \b", 3, HAL_MAX_DELAY);
                }
            }
            // === Обычный печатный символ ===
            else if ((uint8_t)cRxedChar >= 32 && (uint8_t)cRxedChar <= 126)
            {
                if (cInputIndex < (MAX_INPUT_LENGTH - 1))
                {
                    pcInputString[cInputIndex] = cRxedChar;
                    cInputIndex++;
                    pcInputString[cInputIndex] = '\0';

                    // Эхо-печать
                    HAL_UART_Transmit(pxUart, (uint8_t *)&cRxedChar, 1, HAL_MAX_DELAY);
                }
            }
        }
    }
}