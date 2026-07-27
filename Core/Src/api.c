//
// Created by msol0v on 24.07.2026.
//

#include "api.h"


// Вспомогательная функция декодирования URL-символов (%XX и +)
static void url_decode(const char *src, char *dst, size_t maxLen) {
    size_t dstLen = 0;
    while (*src && *src != ' ' && *src != '&' && dstLen < maxLen - 1) {
        if (*src == '%') {
            if (src[1] && src[2]) {
                char hex[3] = {src[1], src[2], '\0'};
                dst[dstLen++] = (char)strtol(hex, NULL, 16);
                src += 3;
            } else {
                break;
            }
        } else if (*src == '+') {
            dst[dstLen++] = ' ';
            src++;
        } else {
            dst[dstLen++] = *src++;
        }
    }
    dst[dstLen] = '\0';
}

// Вспомогательная функция поиска значения аргумента в URL
static bool get_query_param(const char *url, const char *param, char *outVal, size_t maxLen) {
    char searchPattern[32];
    snprintf(searchPattern, sizeof(searchPattern), "%s=", param);

    const char *p = strstr(url, searchPattern);
    if (!p) return false;

    p += strlen(searchPattern);
    size_t len = 0;
    while (*p && *p != '&' && *p != ' ' && len < maxLen - 1) {
        outVal[len++] = *p++;
    }
    outVal[len] = '\0';
    return true;
}

void API_GET_State(char *respJSON, size_t len)
{
    snprintf(respJSON, len,
    "{"
    "\"sid\":%lu,"
    "\"mode\":%u,"
    "\"motor\":%d,"
    "\"cmd\":%d,"
    "\"tA\":%u,"
    "\"tM\":%u,"
    "\"rem\":%u,"
    "\"pr\":%d,"
    "\"tot\":[%lu,%lu,%lu],"
    "\"avg\":[%lu,%lu,%lu],"
    "\"cal\":[%u,%u,%u],"
    "\"s\":[%u,%u,%u,%u,%u,%u],"
    "\"pos\":["
        "[%ld,%ld,%ld],"
        "[%ld,%ld,%ld],"
        "[%ld,%ld,%ld]"
    "],"
    "\"n1\":\"%s\","
    "\"n2\":\"%s\","
    "\"n3\":\"%s\","
    "\"ip\":[%u,%u,%u,%u],"
    "\"sn\":[%u,%u,%u,%u],"
    "\"gw\":[%u,%u,%u,%u]"
    "}",

    (unsigned long)bState.sessionID,
    bState.controlMode,
    bState.selectedMotor,
    bState.lastCmd,
    bState.isActiveAutoTest,
    bState.testMotorsMask,
    bState.testCyclesLeft,
    bState.currentPreset,

    (unsigned long)bState.totalStepsMotors[0],
    (unsigned long)bState.totalStepsMotors[1],
    (unsigned long)bState.totalStepsMotors[2],

    (unsigned long)bState.avgStepsMotors[0],
    (unsigned long)bState.avgStepsMotors[1],
    (unsigned long)bState.avgStepsMotors[2],

    bState.isMotorsCalibrated[0],
    bState.isMotorsCalibrated[1],
    bState.isMotorsCalibrated[2],

    bState.stateGercons[0],
    bState.stateGercons[1],
    bState.stateGercons[2],
    bState.stateGercons[3],
    bState.stateGercons[4],
    bState.stateGercons[5],

    (long)bState.presetPositions[0][0],
    (long)bState.presetPositions[0][1],
    (long)bState.presetPositions[0][2],

    (long)bState.presetPositions[1][0],
    (long)bState.presetPositions[1][1],
    (long)bState.presetPositions[1][2],

    (long)bState.presetPositions[2][0],
    (long)bState.presetPositions[2][1],
    (long)bState.presetPositions[2][2],

    (char *)bState.presetNames[0],
    (char *)bState.presetNames[1],
    (char *)bState.presetNames[2],

    bState.ip[0], bState.ip[1], bState.ip[2], bState.ip[3],
    bState.netmask[0], bState.netmask[1], bState.netmask[2], bState.netmask[3],
    bState.gateway[0], bState.gateway[1], bState.gateway[2], bState.gateway[3]
    );
}

void API_ProcessCommand(const char *urlStr)
{
    char valBuf[64];
    if (!get_query_param(urlStr, "cmd", valBuf, sizeof(valBuf))) {
        return;
    }

    int cmd = atoi(valBuf);
    bState.lastCmd = cmd;

    // Выбор мотора (1..3)
    if (cmd >= 1 && cmd <= 3) {
        API_Cmd_SelectMotor(cmd - 1);
    }
    // Команды шагов ручного управления (5..15)
    else if (cmd >= 5 && cmd <= 15) {
        if (cmd == 10) {
            API_Cmd_Stop();
        } else {
            API_Cmd_Step(cmd);
        }
    }
    // Названия пресетов (151..153)
    else if (cmd >= 151 && cmd <= 153) {
        if (get_query_param(urlStr, "name", valBuf, sizeof(valBuf))) {
            API_Cmd_SetPresetName(cmd - 151, valBuf);
        }
    }
    // Режимы работы (161..163)
    else if (cmd >= 161 && cmd <= 163) {
        API_Cmd_SetMode(cmd - 161);
    }
    // Перейти к пресету (171..173)
    else if (cmd >= 171 && cmd <= 173) {
        API_Cmd_GoPreset(cmd - 171);
    }
    // Сохранить пресет (181..183)
    else if (cmd >= 181 && cmd <= 183) {
        API_Cmd_SavePreset(cmd - 181);
    }
    // Удалить пресет (191..193)
    else if (cmd >= 191 && cmd <= 193) {
        API_Cmd_ClearPreset(cmd - 191);
    }
    // Запуск теста оборудования (200)
    else if (cmd == 200) {
        uint8_t mask = 7;
        uint8_t cycles = 1;
        if (get_query_param(urlStr, "m", valBuf, sizeof(valBuf))) mask = atoi(valBuf);
        if (get_query_param(urlStr, "c", valBuf, sizeof(valBuf))) cycles = atoi(valBuf);
        API_Cmd_RunTest(mask, cycles);
    }
    // Сброс статистики (250)
    else if (cmd == 250) {
        API_Cmd_ResetStats();
    }
    // Настройка сети (300)
    else if (cmd == 300) {
        char ipBuf[16] = {0}, gwBuf[16] = {0}, snBuf[16] = {0};
        get_query_param(urlStr, "ip", ipBuf, sizeof(ipBuf));
        get_query_param(urlStr, "gw", gwBuf, sizeof(gwBuf));
        get_query_param(urlStr, "sn", snBuf, sizeof(snBuf));
        API_Cmd_SaveNetSettings(ipBuf, gwBuf, snBuf);
    }
}

// Заглушки логических обработчиков (реализуются под вашу периферию / FreeRTOS очереди)
void API_Cmd_SetMode(uint8_t mode) {
    bState.controlMode = mode;
}

// Работает только в вебе? Я бы убрал, есть смысл передавать сразу номер моторчика и шаги не отдельными командами, а числом шагов со знаком
void API_Cmd_SelectMotor(int8_t motorIdx) {
    bState.selectedMotor = motorIdx;
}

void API_Cmd_Step(uint8_t stepCmd) {
    bState.currentPreset = -1;
    int8_t steps;
    switch (stepCmd) {
            // НАЗАД
        case 5:  steps = -16;break;
        case 6:  steps = -8; break;
        case 7:  steps = -4; break;
        case 8:  steps = -2; break;
        case 9:  steps = -1; break;

            // ВПЕРЕД
        case 11: steps = 1; break;
        case 12: steps = 2; break;
        case 13: steps = 4; break;
        case 14: steps = 8; break;
        case 15: steps = 16;break;

        default: steps = 0; break;
    }
    // Отправка события в задачу управления шаговыми двигателями через FreeRTOS queue
    MotorCommand_t moveCommand = {
    .motor = bState.selectedMotor,
    .step = steps
    };
    osMessageQueuePut(motorQueueHandle, &moveCommand, 0, 0);
}

void API_Cmd_Stop(void) {
    bState.isActiveAutoTest = 0;
    bState.currentPreset = -1;
    // Аварийная остановка движения, роняем очередь команд
    osMessageQueueReset(motorQueueHandle);
    // Если надо ронять выполнение текущей команды, то надо переделать немного логику выполнения таски моторов для проверки EventFlags
}

// TODO Написать задачу теста
void API_Cmd_RunTest(uint8_t mask, uint8_t cycles) {
    bState.testMotorsMask = mask;
    bState.testCyclesLeft = cycles;
    bState.isActiveAutoTest = 1;
    printf("run test, mask: %d, cycles: %d\r\n", mask, cycles);
    memset(bState.isMotorsCalibrated, 1, sizeof(bState.isMotorsCalibrated));
}

void API_Cmd_ResetStats(void) {
    memset(bState.totalStepsMotors, 0, sizeof(bState.totalStepsMotors));
    memset(bState.avgStepsMotors, 0, sizeof(bState.avgStepsMotors));
    printf("resetstats\r\n");
}

void API_Cmd_SavePreset(uint8_t presetIdx) {
    if (presetIdx < 3) {
        // Сохранить текущие позиции моторов
        bState.currentPreset = -1;
    }
    printf("savepresets id: %d\r\n", presetIdx);
}

void API_Cmd_GoPreset(uint8_t presetIdx) {
    if (presetIdx < 3) {
        bState.currentPreset = presetIdx;
        // Запустить перемещение моторов в записанные координаты
    }
    printf("go preset id: %d\r\n", presetIdx);
}

void API_Cmd_ClearPreset(uint8_t presetIdx) {
    if (presetIdx < 3) {
        for (int i = 0; i < 3; i++) {
            bState.presetPositions[presetIdx][i] = 999999;
        }
        bState.presetNames[presetIdx][0] = '\0';
        bState.currentPreset = -1;
    }
}

void API_Cmd_SetPresetName(uint8_t presetIdx, const char *encodedName) {
    if (presetIdx < 3) {
        url_decode(encodedName, bState.presetNames[presetIdx], sizeof(bState.presetNames[presetIdx]));
    }
    printf("set preset name id: %d, name: %s\r\n", presetIdx, bState.presetNames[presetIdx]);
}

void API_Cmd_SaveNetSettings(const char *ipStr, const char *gwStr, const char *snStr) {
    // Сохранить настройки в Flash/EEPROM и выполнить программную перезагрузку (NVIC_SystemReset)
    printf("savenetsettings ip: %s, gw: %s, mask: %s\r\n", ipStr, gwStr, snStr);
}