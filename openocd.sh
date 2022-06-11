openocd -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32l0.cfg -c "init; program build/ch.hex verify reset exit"
