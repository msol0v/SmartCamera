openocd.exe -f interface/stlink.cfg -f target/stm32f7x.cfg -c "reset_config none separate" -c "set CPUTAPID 0" -c "program build/Debug/SmartCamera.elf reset exit"
