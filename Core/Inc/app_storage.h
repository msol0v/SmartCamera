//
// Created by msol0v on 7/29/26.
//

#ifndef SMARTCAMERA_APP_STORAGE_H
#define SMARTCAMERA_APP_STORAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "main.h" // Файл, где находится объявление typedef struct BoardState_t и extern bState
#include "lfs.h"

typedef struct {
    uint8_t ip[4];
    uint8_t mask[4];
    uint8_t gw[4];
} NetFile_t;

typedef struct {
    int32_t positions[PRESETS_NUM][MOTORS_NUM];
    char    names[PRESETS_NUM][PRESET_NAME_LEN];
} PresetsFile_t;

// API Инициализации и загрузки
bool Storage_Init(void);
void Storage_LoadState(void);

bool Storage_WriteFile(const char *path, const void *buf, size_t size);
bool Storage_ReadFile(const char *path, void *buf, size_t size);

// Асинхронные команды сохранения
void Storage_SaveNetwork(void);
void Storage_SavePresets(void);

bool Storage_IsMounted(void);

#endif //SMARTCAMERA_APP_STORAGE_H
