@echo off
setlocal enabledelayedexpansion

:: Тип сборки по умолчанию — Debug, либо берем из первого аргумента
set PRESET=%~1
if "%PRESET%"=="" set PRESET=Debug

:: Приводим первую букву к заглавной, остальные — к строчным (debug -> Debug, release -> Release)
set "FIRST_CHAR=%PRESET:~0,1%"
set "REST_CHARS=%PRESET:~1%"

for %%A in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
    call :ToCap "%%A"
)

set "PRESET_CAP=%FIRST_CHAR%%REST_CHARS%"
set "ELF_PATH=build\%PRESET_CAP%\SmartCamera.elf"

echo === Прошивка микроконтроллера [%PRESET_CAP%] ===

:: Проверяем, существует ли файл прошивки
if not exist "%ELF_PATH%" (
    echo [ERROR] Ошибка: Файл прошивки '%ELF_PATH%' не найден!
    echo [INFO] Сначала соберите проект: build.bat %PRESET_CAP%. Подробнее в README.md
    exit /b 1
)

:: Запуск OpenOCD
openocd -f interface/stlink.cfg ^
  -f target/stm32f7x.cfg ^
  -c "reset_config none separate" ^
  -c "set CPUTAPID 0" ^
  -c "program %ELF_PATH% reset exit"

if %errorlevel% neq 0 (
    echo [ERROR] Ошибка во время прошивки через OpenOCD!
    exit /b %errorlevel%
)

echo === Прошивка завершена успешно! ===
goto :eof

:: Вспомогательная функция для приведения к регистру
:ToCap
set "CHAR=%~1"
call set "FIRST_CHAR=%%FIRST_CHAR:%CHAR%=%CHAR%%%"
for %%L in (a b c d e f g h i j k l m n o p q r s t u v w x y z) do (
    call set "REST_CHARS=%%REST_CHARS:%%L=%%L%%"
)
goto :eof