#!/usr/bin/env bash
set -e

PRESET=${1:-Debug}

# Если папка сборки еще не создана, сначала запускаем конфигурацию
if [ ! -d "build/$PRESET" ]; then
    echo "=== Первичная конфигурация [$PRESET] ==="
    cmake --preset "$PRESET"
fi

echo "=== Сборка [$PRESET] ==="
cmake --build --preset "$PRESET" -j$(nproc)