# STM32F103C8T6 - Bài tập 03 (CMSIS, không dùng HAL)

# Dùng đường dẫn của chính Makefile để lệnh make chạy được từ mọi thư mục.
PROJECT_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
BUILD       := $(PROJECT_DIR)/build

CUBE        := $(PROJECT_DIR)/../STM32CubeF1
CMSIS       := $(CUBE)/Drivers/CMSIS
DEVICE      := $(CMSIS)/Device/ST/STM32F1xx
CORE_INC    := $(CMSIS)/Include
DEVICE_INC  := $(DEVICE)/Include
APP_INC     := -I$(PROJECT_DIR)/inc

SYSTEM_SRC  := $(DEVICE)/Source/Templates/system_stm32f1xx.c
STARTUP_SRC := $(DEVICE)/Source/Templates/gcc/startup_stm32f103xb.s
LDSCRIPT    := $(PROJECT_DIR)/linker/STM32F103C8.ld

TARGET      := firmware
PREFIX      := arm-none-eabi-
CC          := $(PREFIX)gcc
OBJCOPY     := $(PREFIX)objcopy
SIZE        := $(PREFIX)size

CPU         := -mcpu=cortex-m3 -mthumb
DEFS        := -DSTM32F103xB
INCLUDES    := -I$(CORE_INC) -I$(DEVICE_INC) $(APP_INC)

CFLAGS      := $(CPU) $(DEFS) $(INCLUDES) \
               -std=c11 -O2 -g3 -Wall -Wextra \
               -ffunction-sections -fdata-sections -MMD -MP
ASFLAGS     := $(CPU) -g3
LDFLAGS     := $(CPU) -T$(LDSCRIPT) -Wl,--gc-sections \
               -Wl,-Map=$(BUILD)/$(TARGET).map \
               -specs=nano.specs -specs=nosys.specs

OBJS       := $(BUILD)/main.o \
              $(BUILD)/gpio.o \
              $(BUILD)/led.o \
              $(BUILD)/system_stm32f1xx.o \
              $(BUILD)/startup_stm32f103xb.o


.PHONY: all clean flash

all: $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).bin

$(BUILD):
	mkdir -p "$@"

$(BUILD)/main.o: $(PROJECT_DIR)/src/main.c | $(BUILD)
	$(CC) $(CFLAGS) -c "$<" -o "$@"

$(BUILD)/gpio.o: $(PROJECT_DIR)/src/gpio.c $(PROJECT_DIR)/inc/gpio.h | $(BUILD)
	$(CC) $(CFLAGS) -c "$<" -o "$@"

$(BUILD)/led.o: $(PROJECT_DIR)/src/led.c $(PROJECT_DIR)/inc/led.h $(PROJECT_DIR)/inc/gpio.h | $(BUILD)
	$(CC) $(CFLAGS) -c "$<" -o "$@"

$(BUILD)/system_stm32f1xx.o: $(SYSTEM_SRC) | $(BUILD)
	$(CC) $(CFLAGS) -c "$<" -o "$@"

$(BUILD)/startup_stm32f103xb.o: $(STARTUP_SRC) | $(BUILD)
	$(CC) $(ASFLAGS) -c "$<" -o "$@"

$(BUILD)/$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	$(CC) $(OBJS) $(LDFLAGS) -o "$@"
	$(SIZE) "$@"

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary "$<" "$@"

# Bộ OpenOCD xPack đang có trên máy phát triển. Có thể ghi đè cả hai
# biến trên dòng lệnh nếu cài OpenOCD ở vị trí khác.
OPENOCD_ROOT    ?= D:/Downloads/xpack-openocd-0.12.0-7-win32-x64/xpack-openocd-0.12.0-7
OPENOCD         ?= $(OPENOCD_ROOT)/bin/openocd.exe
OPENOCD_SCRIPTS ?= $(OPENOCD_ROOT)/openocd/scripts

# make chạy trong MSYS biểu diễn ổ D: là /d/. OpenOCD native của Windows
# cần đường dẫn dạng D:/..., nên chuyển riêng đường dẫn file ELF khi flash.
OPENOCD_IMAGE   := $(shell cygpath -m "$(BUILD)/$(TARGET).elf" 2>/dev/null || echo "$(BUILD)/$(TARGET).elf")

flash: all
	"$(OPENOCD)" $(if $(OPENOCD_SCRIPTS),-s "$(OPENOCD_SCRIPTS)",) \
		-f interface/stlink.cfg -f target/stm32f1x.cfg \
		-c "program $(OPENOCD_IMAGE) verify reset exit"

clean:
	rm -rf "$(BUILD)"

-include $(OBJS:.o=.d)
