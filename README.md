# stm-game

'game' is a minimal STM32-based gaming device. The device is based on the [STM32L011F3](https://www.st.com/en/microcontrollers-microprocessors/stm32l011f3.html), with only 16kB of flash an 2kB of RAM, and includes an [EA DOGS102N-6](https://www.digikey.com/en/products/detail/display-visions/EA-DOGS102N-6/4896732) low-power LCD display with a digital joystick and some buttons. A CR2032 battery is used for power.

The primary goal of this project is to master low-power techniques: the device currently plays games with only 340uA of current consumption, dropping to about 272uA while sleeping (display on). This has been achieved through the use of low-power modes, varying clock speeds, and an interrupt-driven software architecture.

The first game written for this device is a "Flappy Bird" clone. More games will be developed and added to the project over time.
