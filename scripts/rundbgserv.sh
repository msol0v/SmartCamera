openocd -f interface/stlink.cfg \
  -f target/stm32f7x.cfg \
  -c "reset_config none separate" \
  -c "set CPUTAPID 0"
