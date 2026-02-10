LIB_NAME := STM32F4
BUILD ?= debug
BUILD_DIR := build/$(BUILD)

TOOL_DIR ?= /opt/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi/bin/
CC := $(TOOL_DIR)/arm-none-eabi-gcc
AR := $(TOOL_DIR)/arm-none-eabi-ar

SRC := $(wildcard Src/*.c)
OBJ := $(patsubst Src/%.c,$(BUILD_DIR)/%.o,$(SRC))

CFLAGS_COMMON := \
  -mcpu=cortex-m4 \
  -std=gnu11 \
  -DSTM32 -DSTM32F4 -DSTM32F411CEUx \
  -c \
  -IInc \
  -ffunction-sections \
  -fdata-sections \
  -Wall -Wextra \
  -v -fstack-usage \
  -mfpu=fpv4-sp-d16 \
  -mfloat-abi=hard \
  -mthumb

CFLAGS_DEBUG := -O0 -g3 -DDEBUG
CFLAGS_RELEASE := -O2 -DNDEBUG

ifeq ($(BUILD), debug)
	CFLAGS := $(CFLAGS_COMMON) $(CFLAGS_DEBUG)
else ifeq ($(BUILD), release)
	CFLAGS := $(CFLAGS_COMMON) $(CFLAGS_RELEASE)
else
	$(error Unknown BUILD=$(BUILD))
endif

all: build-one

build-one: $(BUILD_DIR)/lib$(LIB_NAME).a

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: Src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/lib$(LIB_NAME).a: $(OBJ)
	$(AR) rcs $@ $^

clean:
	rm -rf build/

.PHONY: all clean debug release build-one
