openocd -f /usr/local/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/local/share/openocd/scripts/target/stm32l0.cfg -c "init; program build/ch.hex verify reset exit"
