all: build-one

debug:
	@$(MAKE) --no-print-directory BUILD=debug build-one

release:
	@$(MAKE) --no-print-directory BUILD=release build-one

clean:
	rm -rf build/

LIB_NAME := STM32F4
BUILD ?= debug
BUILD_DIR := build/$(BUILD)

TOOL_DIR ?= /opt/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi/bin
CC := $(TOOL_DIR)/arm-none-eabi-gcc
AR := $(TOOL_DIR)/arm-none-eabi-ar

SRC := $(wildcard drivers/*.c core/*.c)
OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))
DIRS := $(sort $(dir $(OBJ)))

CFLAGS_COMMON := \
  -mcpu=cortex-m4 \
  -std=gnu11 \
  -DSTM32 -DSTM32F4 -DSTM32F411CEUx \
  -IInc \
  -ffunction-sections \
  -fdata-sections \
  -Wall -Wextra \
  -v \
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

build-one: $(BUILD_DIR)/lib$(LIB_NAME).a

$(DIRS):
	mkdir -p $@

$(BUILD_DIR)/%.o: %.c | $(DIRS)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/lib$(LIB_NAME).a: $(OBJ) | $(BUILD_DIR)
	$(AR) rcs $@ $^

.PHONY: all clean debug release
