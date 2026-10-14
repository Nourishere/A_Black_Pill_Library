# A Baremetal Library for STM32F401xB/C Based Boards
* This repository is intended to offer a stable baremetal static library for the STM32F401xB/C Cortex-M4 based microcontrollers with future plans to port it to other Cortex-M devices.
* The board I am using for testing is the **WeAct Black Pill V1.2**, which has a STM32F401CCU6 MCU at its heart.

## Repository Structure

```
.
├── README.md
├── LICENSE 
├── Makefile
├──	core/ (Cortex M specific)
│   ├── nvic.c
|   └── ...
├── drivers/
│   ├── gpio.c
|   └── ...
|
└──	Inc/
    ├── core/
	|	 ├── nvic.h
	|    └── ...
    └── drivers/
		 ├── gpio.h
	     └── ...
```

## Prerequisites 
* You need to have `make` and the `arm-none-eabi` GCC toolchain or something similar installed, or use an IDE if you are into that kind of stuff!
 
## Building
* Use `make` to build the library; `make` defaults to building a debug lib version.
* Use `make release` to build a library with compiler optimizations and no debug flags. 
* Use `make TOOL_DIR=/path/to/toolchain/bin` to specify the toolchain directory. 

## Integration
* You can use the library by linking it with any other project you're working on.

## Contribution
* Pull requests are welcome.
