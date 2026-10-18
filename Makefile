TARGET = firmware

SRC_DIR   = src
INC_DIR   = inc
LIB_DIR   = Lib
BUILD_DIR = proj

SPL_DIR      = $(LIB_DIR)/STM32F10x_StdPeriph_Driver
CMSIS_DIR    = $(LIB_DIR)/CMSIS/CM3
FREERTOS_DIR = FreeRTOS

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

STINFO  = st-info
STFLASH = st-flash

CPU = -mcpu=cortex-m3 -mthumb

DEFS = \
-DSTM32F10X_MD \
-DUSE_STDPERIPH_DRIVER

INCLUDES = \
-I$(INC_DIR) \
-I$(SPL_DIR)/inc \
-I$(CMSIS_DIR)/CoreSupport \
-I$(CMSIS_DIR)/DeviceSupport/ST/STM32F10x \
-I$(FREERTOS_DIR)/include \
-I$(FREERTOS_DIR)/portable/ARM_CM3

CFLAGS = \
$(CPU) \
$(DEFS) \
$(INCLUDES) \
-ffunction-sections \
-fdata-sections

LDFLAGS = \
$(CPU) \
-nostartfiles \
-T linker.ld \
-Wl,--gc-sections

APP_SOURCES := $(wildcard $(SRC_DIR)/*.c)
SPL_SOURCES := $(wildcard $(SPL_DIR)/src/*.c)

SYSTEM_SOURCE = \
$(CMSIS_DIR)/DeviceSupport/ST/STM32F10x/system_stm32f10x.c

STARTUP_SOURCE = \
$(CMSIS_DIR)/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_md.s

FREERTOS_SOURCES := $(wildcard $(FREERTOS_DIR)/*.c)
FREERTOS_SOURCES += $(FREERTOS_DIR)/portable/ARM_CM3/port.c
FREERTOS_SOURCES += $(FREERTOS_DIR)/portable/MemMang/heap_4.c
APP_OBJECTS := \
$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(APP_SOURCES))

SPL_OBJECTS := \
$(patsubst $(SPL_DIR)/src/%.c,$(BUILD_DIR)/spl_%.o,$(SPL_SOURCES))

SYSTEM_OBJECT  = $(BUILD_DIR)/system_stm32f10x.o
STARTUP_OBJECT = $(BUILD_DIR)/startup.o

FREERTOS_OBJECTS := $(patsubst $(FREERTOS_DIR)/%.c,$(BUILD_DIR)/freertos_%.o,$(filter-out $(FREERTOS_DIR)/portable/%,$(FREERTOS_SOURCES)))
FREERTOS_OBJECTS += $(BUILD_DIR)/freertos_port.o
FREERTOS_OBJECTS += $(BUILD_DIR)/freertos_heap_4.o

OBJECTS = \
$(APP_OBJECTS) \
$(SPL_OBJECTS) \
$(SYSTEM_OBJECT) \
$(STARTUP_OBJECT) \
$(FREERTOS_OBJECTS)

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR):
ifeq ($(OS),Windows_NT)
	powershell -NoProfile -Command "New-Item -ItemType Directory -Force -Path '$(BUILD_DIR)' | Out-Null"
else
	mkdir -p $(BUILD_DIR)
endif

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/spl_%.o: $(SPL_DIR)/src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(SYSTEM_OBJECT): $(SYSTEM_SOURCE) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(STARTUP_OBJECT): $(STARTUP_SOURCE) | $(BUILD_DIR)
	$(CC) $(CPU) -c $< -o $@

$(BUILD_DIR)/freertos_%.o: $(FREERTOS_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/freertos_port.o: $(FREERTOS_DIR)/portable/ARM_CM3/port.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/freertos_heap_4.o: $(FREERTOS_DIR)/portable/MemMang/heap_4.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	$(SIZE) $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

check:
	$(STINFO) --probe

flash: $(BUILD_DIR)/$(TARGET).bin
	$(STINFO) --probe
	$(STFLASH) write $(BUILD_DIR)/$(TARGET).bin 0x08000000

clean:
ifeq ($(OS),Windows_NT)
	powershell -NoProfile -Command "if (Test-Path '$(BUILD_DIR)') { Remove-Item -Recurse -Force '$(BUILD_DIR)' }"
else
	rm -rf $(BUILD_DIR)
endif

.PHONY: all clean check flash