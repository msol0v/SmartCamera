//
// Created by msol0v on 28.07.2026.
//
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "lwip.h"

#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/dhcp.h"

#include "main.h"
#include "pwm_task.h"

static BaseType_t prvResetMCUCallback(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    __disable_irq();
    NVIC_SystemReset();
    while(1);
}

static BaseType_t prvStepCallback(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    BaseType_t xParamLen1, xParamLen2;;
    const char *pcParam1, *pcParam2;

    pcParam1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParamLen1);
    pcParam2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParamLen2);

    if (pcParam1 == NULL || pcParam2 == NULL)
    {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Ошибка: требуется 2 аргумента: <мотор (0-2)> <шаг (-16..16)>\r\n");
        return pdFALSE;
    }

    char *endptr1, *endptr2;
    long val1 = strtol(pcParam1, &endptr1, 10);
    long val2 = strtol(pcParam2, &endptr2, 10);

    // Проверка: были ли параметры действительными числами
    if ((endptr1 == pcParam1) || (endptr2 == pcParam2))
    {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Ошибка: аргументы должны быть целыми числами!\r\n");
        return pdFALSE;
    }

    // 4. Проверяем диапазоны
    // Первый аргумент: от 0 до 2
    if (val1 < 0 || val1 > 2)
    {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Ошибка: 1-й аргумент (мотор) должен быть от 0 до 2 (передано: %ld)\r\n", val1);
        return pdFALSE;
    }

    // // Второй аргумент: от -16 до +16
    // if (val2 < -16 || val2 > 16)
    // {
    //     snprintf(pcWriteBuffer, xWriteBufferLen, "Ошибка: 2-й аргумент (шаг) должен быть от -16 до +16 (передано: %ld)\r\n", val2);
    //     return pdFALSE;
    // }

    // 5. Выполняем полезное действие (здесь логика вашей программы)
    uint8_t motor = (uint8_t)val1;
    int8_t steps = (int8_t)val2;
    MotorCommand_t msg = {
        .motor = motor,
        .step = steps
    };

    osMessageQueuePut(motorQueueHandle, &msg, 0, 0);

    return pdFALSE;
}

static BaseType_t prvModeMCUCallback(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    BaseType_t xParamLen;
    const char *pcParam = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParamLen);
    if (pcParam == NULL) {
        const char *modeStr = "UNKNOWN";

        switch (bState.controlMode) {
            case CONTROL_MODE_MVS: modeStr = "MVS"; break;
            case CONTROL_MODE_PLC: modeStr = "PLC"; break;
            case CONTROL_MODE_WEB: modeStr = "WEB"; break;
            default: break;
        }

        snprintf(pcWriteBuffer, xWriteBufferLen, "Current mode: %s\r\n", modeStr);
        return pdFALSE;
    }

    // Сравниваем полученную строку с ожидаемыми значениями
    if (strncmp(pcParam, "MVS", xParamLen) == 0 && xParamLen == 3)
    {
        stopCurrentModeRead();
        bState.controlMode = CONTROL_MODE_MVS;
        startCurrendModeRead();
        snprintf(pcWriteBuffer, xWriteBufferLen, "Mode changed to MVS\r\n");
    }
    else if (strncmp(pcParam, "PLC", xParamLen) == 0 && xParamLen == 3)
    {
        stopCurrentModeRead();
        bState.controlMode = CONTROL_MODE_PLC;
        startCurrendModeRead();
        snprintf(pcWriteBuffer, xWriteBufferLen, "Mode changed to PLC\r\n");
    }
    else if (strncmp(pcParam, "WEB", xParamLen) == 0 && xParamLen == 3)
    {
        stopCurrentModeRead();
        bState.controlMode = CONTROL_MODE_WEB;
        snprintf(pcWriteBuffer, xWriteBufferLen, "Mode changed to WEB\r\n");
    }
    else
    {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Error: Unknown mode. Use MVS, PLC or WEB.\r\n");
    }
    return pdFALSE;
}


static void prvEnableDHCP(void)
{
    if (netif_default != NULL)
    {
        // // Останавливаем старую работу и освобождаем адрес
        // dhcp_release(netif_default);
        // dhcp_stop(netif_default);
        //
        // //Сброс IP чтобы таска проверки знала, что мы ждем новый адрес
        // ip_addr_t zero_ip = IPADDR4_INIT(0);
        // netif_set_ipaddr(netif_default, ip_2_ip4(&zero_ip));
        //
        // //Запускаем DHCP
        // dhcp_start(netif_default);
        //
        // //Будим поток отслеживания
        // startDhcpCheckTask();
    }
}

static void prvSetStaticIP(const char *pcIP, const char *pcMask, const char *pcGW)
{
    ip4_addr_t ipaddr, netmask, gw;

    if (netif_default != NULL &&
        ip4addr_aton(pcIP, &ipaddr) &&
        ip4addr_aton(pcMask, &netmask) &&
        ip4addr_aton(pcGW, &gw))
    {
        // dhcp_stop(netif_default);
        netif_set_addr(netif_default, &ipaddr, &netmask, &gw);
    }
}

static BaseType_t prvIpCommandCallback(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    const char *pcParam1;
    const char *pcParam2;
    const char *pcParam3;
    BaseType_t xParam1Len, xParam2Len, xParam3Len;

    if (netif_default == NULL)
    {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Error: Network interface not initialized!\r\n");
        return pdFALSE;
    }

    pcParam1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParam1Len);

    if (pcParam1 == NULL)
    {
        // Вывод текущего IP
        if (netif_is_up(netif_default) && netif_default->ip_addr.addr != 0)
        {
            char cIP[16], cMask[16], cGW[16];

            ip4addr_ntoa_r(netif_ip4_addr(netif_default), cIP, sizeof(cIP));
            ip4addr_ntoa_r(netif_ip4_netmask(netif_default), cMask, sizeof(cMask));
            ip4addr_ntoa_r(netif_ip4_gw(netif_default), cGW, sizeof(cGW));

            // snprintf(pcWriteBuffer, xWriteBufferLen,
            //          "Current IP: %s\r\nMask: %s\r\nGW: %s\r\nMode: %s\r\n",
            //          cIP,
            //          cMask,
            //          cGW,
            //          dhcp_supplied_address(netif_default) ? "DHCP" : "Static");
        }
        else
        {
            snprintf(pcWriteBuffer, xWriteBufferLen, "IP Address: Link down or not assigned\r\n");
        }
    }
    else if (strncmp(pcParam1, "dhcp", xParam1Len) == 0)
    {
        prvEnableDHCP();
        snprintf(pcWriteBuffer, xWriteBufferLen, "Requesting IP via DHCP...\r\n");
    }
    else if (strncmp(pcParam1, "reset-phy", xParam1Len) == 0)
    {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Resetting PHY...\r\n");

        //Уведомляем LwIP, что интерфейс пдает
        if (netif_default != NULL)
        {
            netif_set_link_down(netif_default);
            netif_set_down(netif_default);
        }

        // Аппаратный сброс пина PHY (RESET = LOW)
        HAL_GPIO_WritePin(RESET_PHY_GPIO_Port, RESET_PHY_Pin, GPIO_PIN_RESET);
        vTaskDelay(pdMS_TO_TICKS(100));
        HAL_GPIO_WritePin(RESET_PHY_GPIO_Port, RESET_PHY_Pin, GPIO_PIN_SET);

        //Пауза для внутренней инициализации PHY
        vTaskDelay(pdMS_TO_TICKS(200));

        //Переинициализация
        HAL_ETH_DeInit(&heth);
        if (HAL_ETH_Init(&heth) == HAL_OK)
        {
            //Возвращаем LwIP в рабочее состояние
            if (netif_default != NULL)
            {
                netif_set_up(netif_default);

                // Если включен DHCP перезапускаем его
                // if (dhcp_supplied_address(netif_default))
                // {
                //     prvEnableDHCP();
                // }
                // else
                // {
                    netif_set_link_up(netif_default);
                //}
            }
            snprintf(pcWriteBuffer, xWriteBufferLen, "PHY Reset Complete. Link restoring...\r\n");
        }
        else
        {
            snprintf(pcWriteBuffer, xWriteBufferLen, "Error: HAL_ETH_Init failed after PHY reset!\r\n");
        }
    }
    else
    {
        pcParam2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParam2Len);
        pcParam3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &xParam3Len);

        if (pcParam2 != NULL && pcParam3 != NULL)
        {
            char cIP[16] = {0}, cMask[16] = {0}, cGW[16] = {0};

            strncpy(cIP, pcParam1, xParam1Len < 16 ? xParam1Len : 15);
            strncpy(cMask, pcParam2, xParam2Len < 16 ? xParam2Len : 15);
            strncpy(cGW, pcParam3, xParam3Len < 16 ? xParam3Len : 15);

            prvSetStaticIP(cIP, cMask, cGW);

            snprintf(pcWriteBuffer, xWriteBufferLen, "Static IP updated to %s\r\n", cIP);
        }
        else
        {
            snprintf(pcWriteBuffer, xWriteBufferLen, "Usage:\r\n ip dhcp\r\n ip <ip> <mask> <gw>\r\n");
        }
    }

    return pdFALSE;
}

/* ##############       Дефинишены команд       ##########################*/

static const CLI_Command_Definition_t xResetCommandDefinition = {
    .pcCommand                   = "reset",
    .pcHelpString                = "reset:\r\n"
                                   "    Software MCU Reset.\r\n",
    .pxCommandInterpreter        = prvResetMCUCallback,
    .cExpectedNumberOfParameters = 0 // Принимает от 0 до 3 параметров
};

//Описание структуры команды
static const CLI_Command_Definition_t xIpCommandDefinition = {
    .pcCommand                   = "ip",
    .pcHelpString                = "ip:\r\n"
                                   "  Get or set network IP configuration.\r\n"
                                   "  Usage:\r\n"
                                   "    ip                           - Show current IP address and status\r\n"
                                   "    ip dhcp                      - Enable DHCP and request new IP\r\n"
                                   "    ip <ip> <mask> <gateway>     - Set static IP (e.g. ip 192.168.1.50 255.255.255.0 192.168.1.1)\r\n"
                                   "    ip reset-phy                 - Do hw reset phy\r\n",
    .pxCommandInterpreter        = prvIpCommandCallback,
    .cExpectedNumberOfParameters = -1 // Принимает от 0 до 3 параметров
};

static const CLI_Command_Definition_t xModeCommandDefinition = {
    .pcCommand                   = "mode",
    .pcHelpString                = "mode:\r\n"
                                   "    Select board control mode\r\n"
                                   "    Usage:\r\n"
                                   "        mode <MVS|PLC|WEB>\r\n",
    .pxCommandInterpreter        = prvModeMCUCallback,
    .cExpectedNumberOfParameters = -1
};

static const CLI_Command_Definition_t xStepCommandDefinition = {
    .pcCommand                   = "step",
    .pcHelpString                = "step:\r\n"
                                   "    Do step motor\r\n"
                                   "    Usage:\r\n"
                                   "        step <motor> <steps>",
    .pxCommandInterpreter        = prvStepCallback,
    .cExpectedNumberOfParameters = -1
};

// Публичная функция регистрации
void vRegisterCommands(void)
{
    FreeRTOS_CLIRegisterCommand(&xIpCommandDefinition);
    FreeRTOS_CLIRegisterCommand(&xResetCommandDefinition);
    FreeRTOS_CLIRegisterCommand(&xModeCommandDefinition);
    FreeRTOS_CLIRegisterCommand(&xStepCommandDefinition);
}