//
// Created by msol0v on 7/29/26.
//

#ifndef SMARTCAMERA_LFS_W25Q_ADAPTER_H
#define SMARTCAMERA_LFS_W25Q_ADAPTER_H

#include "lfs.h"
#include "w25q_mem.h"

// Конфигурация и структура ФС доступны глобально
extern const struct lfs_config LFS_CFG;
extern lfs_t lfs;

/**
 * @brief Инициализация и монтирование LittleFS.
 *        Форматирует Flash автоматически, если ФС еще не создана.
 * @return W25Q_OK если успешно, W25Q_CHIP_ERR при ошибке
 */
W25Q_STATE LFS_W25Q_Init(void);

#endif //SMARTCAMERA_LFS_W25Q_ADAPTER_H
