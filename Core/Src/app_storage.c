//
// Created by msol0v on 7/29/26.
//

#include "app_storage.h"
#include "../Inc/w25q_mem.h"

extern lfs_t lfs;
extern bool LFS_W25Q_Init(void);
extern int32_t getCurrentMotorPosition(uint8_t motorIdx); // Ваша функция из драйвера двигателей
static bool g_storage_mounted = false;

bool Storage_ReadFile(const char *path, void *buf, size_t size) {
    lfs_file_t file;
    // LittleFS сама залочит мьютекс внутри lfs_file_open / read / close
    int err = lfs_file_open(&lfs, &file, path, LFS_O_RDONLY);
    if (err < 0) return false;

    lfs_ssize_t res = lfs_file_read(&lfs, &file, buf, size);
    lfs_file_close(&lfs, &file);

    return (res == (lfs_ssize_t)size);
}

bool Storage_WriteFile(const char *path, const void *buf, size_t size) {
    lfs_file_t file;
    int err = lfs_file_open(&lfs, &file, path, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
    if (err < 0) return false;

    lfs_ssize_t res = lfs_file_write(&lfs, &file, buf, size);
    lfs_file_close(&lfs, &file);

    return (res == (lfs_ssize_t)size);
}

// Проверка монтирования фс
bool Storage_IsMounted(void) {
    return g_storage_mounted;
}

// Инициализация файловой системы
bool Storage_Init(void) {
    /* Вот тут напоролся. Если будете изменять как-то связь драйвера и ФС
     *  то не возвращайте значения драйвера в колбеках, они разные
     *  ОК драйвера может быть ошибкой ФС
     */
    if (LFS_W25Q_Init() == W25Q_OK){
        g_storage_mounted = true;
        return true;
    }
    return false;
}

// Загрузка состояния из LittleFS или установка Дефолтов
void Storage_LoadState(void) {
   NetFile_t net;

    if (Storage_ReadFile("net.bin", &net, sizeof(net))) {
        memcpy((void *)bState.ip, net.ip, 4);
        memcpy((void *)bState.netmask, net.mask, 4);
        memcpy((void *)bState.gateway, net.gw, 4);
    } else {
        // Дефолтный IP если файл отсутствует
        bState.ip[0] = 192; bState.ip[1] = 168; bState.ip[2] = 1; bState.ip[3] = 177;
        bState.netmask[0] = 255; bState.netmask[1] = 255; bState.netmask[2] = 255; bState.netmask[3] = 0;
        bState.gateway[0] = 192; bState.gateway[1] = 168; bState.gateway[2] = 1; bState.gateway[3] = 1;
        Storage_SaveNetwork();
    }

    // Пресеты
    struct {
        int32_t positions[PRESETS_NUM][3];
        char    names[PRESETS_NUM][PRESET_NAME_LEN];
    } presets;

    if (Storage_ReadFile("presets.bin", &presets, sizeof(presets))) {
        memcpy((void *)bState.presetPositions, presets.positions, sizeof(bState.presetPositions));
        memcpy((void *)bState.presetNames, presets.names, sizeof(bState.presetNames));
    } else {
        // Дефолтные пресеты
        for (int i = 0; i < PRESETS_NUM; i++) {
            for (int m = 0; m < 3; m++) {
                bState.presetPositions[i][m] = 0;
            }
            snprintf((void *)bState.presetNames[i], PRESET_NAME_LEN, "Preset %d", i + 1);
        }
        Storage_SavePresets();
    }
}

// Сохранение сети
void Storage_SaveNetwork(void) {
    NetFile_t net;

    memcpy(net.ip, (const void *)bState.ip, 4);
    memcpy(net.mask, (const void *)bState.netmask, 4);
    memcpy(net.gw, (const void *)bState.gateway, 4);

    Storage_WriteFile("net.bin", &net, sizeof(net));
}

// Сохранение пресетов
void Storage_SavePresets(void) {
    PresetsFile_t presets;

    memcpy(presets.positions, (const void *)bState.presetPositions, sizeof(bState.presetPositions));
    memcpy(presets.names, (const void *)bState.presetNames, sizeof(bState.presetNames));

    Storage_WriteFile("presets.bin", &presets, sizeof(presets));
}