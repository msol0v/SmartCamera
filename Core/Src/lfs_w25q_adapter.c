//
// Created by msol0v on 7/29/26.
//
#include "lfs_w25q_adapter.h"
#include "FreeRTOS.h"
#include "semphr.h"

lfs_t lfs;

// Буферы для исключения динамической памяти (heap)
static uint8_t lfs_read_buf[MEM_PAGE_SIZE];
static uint8_t lfs_prog_buf[MEM_PAGE_SIZE];
static uint8_t lfs_lookahead_buf[16];

// Статический мьютекс FreeRTOS для ядра LittleFS
static SemaphoreHandle_t lfs_mutex = NULL;

// Callback: Захват мьютекса
static int lfs_w25q_lock(const struct lfs_config *c) {
    (void)c;
    if (lfs_mutex == NULL) {
        return LFS_ERR_IO;
    }
    // Ожидаем мьютекс бесконечно (в контексте задач FreeRTOS)
    if (xSemaphoreTake(lfs_mutex, portMAX_DELAY) == pdTRUE) {
        return LFS_ERR_OK;
    }
    return LFS_ERR_IO;
}

// Callback: Освобождение мьютекса
static int lfs_w25q_unlock(const struct lfs_config *c) {
    (void)c;
    if (lfs_mutex == NULL) {
        return LFS_ERR_IO;
    }
    if (xSemaphoreGive(lfs_mutex) == pdTRUE) {
        return LFS_ERR_OK;
    }
    return LFS_ERR_IO;
}

// Callback: Чтение сырых байт
static int lfs_w25q_read(const struct lfs_config *c, lfs_block_t block,
                         lfs_off_t off, void *buffer, lfs_size_t size) {
    uint32_t raw_addr = (block * c->block_size) + off;
    if (W25Q_ReadRaw((uint8_t*)buffer, (uint16_t)size, raw_addr) == W25Q_OK) {
        return LFS_ERR_OK;
    }
    return LFS_ERR_IO;
}

// Callback: Запись (программирование)
static int lfs_w25q_prog(const struct lfs_config *c, lfs_block_t block,
                         lfs_off_t off, const void *buffer, lfs_size_t size) {
    uint32_t raw_addr = (block * c->block_size) + off;
    W25Q_STATE res = W25Q_ProgramRaw((uint8_t*)buffer, (uint16_t)size, raw_addr);

    // Нужно прокинуть выводы драйвера чтобы потом не искать долго где ошибка,
    // просто трейс файловой системы включить и в консоли она сама все расскажет кто есть кто
    switch (res) {
    case W25Q_OK:        return LFS_ERR_OK;
    case W25Q_PARAM_ERR: return LFS_ERR_INVAL; // Ошибка аргументов
    case W25Q_SPI_ERR:   return LFS_ERR_IO;    // Ошибка шины
    default:             return LFS_ERR_IO;
    }
}

// Callback: Стирание сектора (4 KB)
static int lfs_w25q_erase(const struct lfs_config *c, lfs_block_t block) {
    (void)c;
    // Передаем номер блока напрямую, так как W25Q_EraseSector
    // сама умножает его на (MEM_SECTOR_SIZE * 1024U) внутри себя!!!!!!
    if (W25Q_EraseSector((uint32_t)block) == W25Q_OK) {
        return LFS_ERR_OK;
    }
    return LFS_ERR_IO;
}

// Callback: Проверка занятости чипа
static int lfs_w25q_sync(const struct lfs_config *c) {
    (void)c;
    // Ждем, пока Flash закончит внутренние операции записи/стирания
    while (W25Q_IsBusy() == W25Q_BUSY) {
        vTaskDelay(pdMS_TO_TICKS(1)); // Или w25q_delay(1);
    }
    return LFS_ERR_OK;
}


// Конфигурация под параметры W25Q64 из w25q_mem.h
const struct lfs_config LFS_CFG = {
    .read  = lfs_w25q_read,
    .prog  = lfs_w25q_prog,
    .erase = lfs_w25q_erase,
    .sync  = lfs_w25q_sync,

    // Передаем функции блокировки для ядра LittleFS
    .lock   = lfs_w25q_lock,
    .unlock = lfs_w25q_unlock,

    .read_size      = 16,
    .prog_size      = MEM_PAGE_SIZE,
    .block_size     = (MEM_SECTOR_SIZE * 1024U),
    .block_count    = SECTOR_COUNT,
    .cache_size     = MEM_PAGE_SIZE,
    .lookahead_size = 16,
    .block_cycles   = 500,

    .read_buffer      = lfs_read_buf,
    .prog_buffer      = lfs_prog_buf,
    .lookahead_buffer = lfs_lookahead_buf,
};

W25Q_STATE LFS_W25Q_Init(void) {
    // 1. Создаем мьютекс FreeRTOS перед монтированием
    if (lfs_mutex == NULL) {
        lfs_mutex = xSemaphoreCreateMutex();
        if (lfs_mutex == NULL) {
            return W25Q_CHIP_ERR;
        }
    }

    if (W25Q_Init() != W25Q_OK) {
        return W25Q_CHIP_ERR;
    }

    memset(&lfs, 0, sizeof(lfs));

    int err = lfs_mount(&lfs, &LFS_CFG);
    if (err) {
        memset(&lfs, 0, sizeof(lfs));
        err = lfs_format(&lfs, &LFS_CFG);
        if (err) return W25Q_CHIP_ERR;

        memset(&lfs, 0, sizeof(lfs));
        err = lfs_mount(&lfs, &LFS_CFG);
        if (err) return W25Q_CHIP_ERR;
    }

    return W25Q_OK;
}