#!/usr/bin/env bash
set -e

# Тип сборки по умолчанию — Debug, либо берем из первого аргумента
PRESET=${1:-Debug}

# Приводим к красивому регистру (первая буква заглавная)
PRESET_CAP="$(tr '[:lower:]' '[:upper:]' <<< "${PRESET:0:1}")${PRESET:1}"

ELF_PATH="build/${PRESET_CAP}/SmartCamera.elf"

echo "=== Прошивка микроконтроллера [ $PRESET_CAP ] ==="

# Проверяем, существует ли файл прошивки
if [ ! -f "$ELF_PATH" ]; then
    echo "❌ Ошибка: Файл прошивки '$ELF_PATH' не найден!"
    echo "💡 Сначала соберите проект: ./script/build<.sh|.bat> <Debug | Release>. Подробнее в README.md"
    exit 1
fi

# Запуск OpenOCD
openocd -f interface/stlink.cfg \
  -f target/stm32f7x.cfg \
  -c "reset_config none separate" \
  -c "set CPUTAPID 0" \
  -c "program ${ELF_PATH} reset exit"

echo "=== Прошивка завершена успешно! ==="