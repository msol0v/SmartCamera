@echo off
setlocal enabledelayedexpansion

:: Тип сборки по умолчанию — Debug, либо берем из первого аргумента
set PRESET=%~1
if "%PRESET%"=="" set PRESET=Debug

:: Проверяем, существует ли папка сборки для этого пресета
if not exist "build\%PRESET%" (
    echo === Первичная конфигурация [%PRESET%] ===
    cmake --preset "%PRESET%"
    if %errorlevel% neq 0 (
        echo [ERROR] Ошибка конфигурации CMake!
        exit /b %errorlevel%
    )
)

echo === Сборка [%PRESET%] ===
cmake --build --preset "%PRESET%"

if %errorlevel% neq 0 (
    echo [ERROR] Ошибка сборки!
    exit /b %errorlevel%
)

echo === Сборка успешно завершена! ===