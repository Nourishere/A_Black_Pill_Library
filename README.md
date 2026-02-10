# A Baremetal Library for the Black Pill
* This repository is intended to offer a stable baremetal static library for the STM32F411CEU6 MCU (AKA the Black Pill.)

## Repository Structure

```
.
├── README.md
├── Makefile
├── Inc
│   ├── GPIO
│   |    ├── GPIO.h
|   |    └── ...
|   └── ...
|
└── Src
    ├── GPIO
    |    ├── GPIO.c
    |    └── ...
    └── ...
```
	
## Prerequisites 
* You need to have `make` and the `arm-none-eabi` GCC toolchain or something similar installed, or use an IDE if you are into that kind of stuff!
 
## Building
* Use `make` to bulid the library; `make` defaults to bulilding a debug lib version.
* Use `make release` to build a library with compiler optimizations and no debug flags. 
* Use `make TOOL_DIR=/path/to/toolchain/bin` to specify the toolchain directory. 

## Integration
* You can ues the library by linking it with any other project you're working on.

## Contribution
* Pull requests are welcome.
