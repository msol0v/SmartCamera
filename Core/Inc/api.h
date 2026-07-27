//
// Created by msol0v on 24.07.2026.
//

#ifndef SMARTCAMERA_API_H
#define SMARTCAMERA_API_H

#include "router.h"
#include "main.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void API_GET_State(char *respJSON, size_t len);
void API_ProcessCommand(const char *urlStr);

// Функции записи параметров
void API_Cmd_SetMode(uint8_t mode);
void API_Cmd_SelectMotor(int8_t motorIdx);
void API_Cmd_Step(uint8_t stepCmd);
void API_Cmd_Stop(void);
void API_Cmd_RunTest(uint8_t mask, uint8_t cycles);
void API_Cmd_ResetStats(void);
void API_Cmd_SavePreset(uint8_t presetIdx);
void API_Cmd_GoPreset(uint8_t presetIdx);
void API_Cmd_ClearPreset(uint8_t presetIdx);
void API_Cmd_SetPresetName(uint8_t presetIdx, const char *encodedName);
void API_Cmd_SaveNetSettings(const char *ipStr, const char *gwStr, const char *snStr);

#endif //SMARTCAMERA_API_H