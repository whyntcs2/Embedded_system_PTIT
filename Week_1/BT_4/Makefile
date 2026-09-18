TARGET = firmware

SRC_DIR   = src
INC_DIR   = inc
LIB_DIR   = Lib
BUILD_DIR = proj

SPL_DIR = $(LIB_DIR)/Libraries/STM32F10x_StdPeriph_Driver
CMSIS_DIR = $(LIB_DIR)/Libraries/CMSIS/CM3

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

CPU = -mcpu=cortex-m3 -mthumb

DEFS = \
-DSTM32F10X_MD \
-DUSE_STDPERIPH_DRIVER

INCLUDES = \
-I$(INC_DIR) \
-I$(SPL_DIR)/inc \
-I$(CMSIS_DIR)/CoreSupport \
-I$(CMSIS_DIR)/DeviceSupport/ST/STM32F10x

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

# Tự động tìm tất cả file .c do người dùng viết
APP_SOURCES := $(wildcard $(SRC_DIR)/*.c)

# Tự động tìm toàn bộ source SPL
SPL_SOURCES := $(wildcard $(SPL_DIR)/src/*.c)

# CMSIS system source
SYSTEM_SOURCE = \
$(CMSIS_DIR)/DeviceSupport/ST/STM32F10x/system_stm32f10x.c

APP_OBJECTS := \
$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(APP_SOURCES))

SPL_OBJECTS := \
$(patsubst $(SPL_DIR)/src/%.c,$(BUILD_DIR)/spl_%.o,$(SPL_SOURCES))

SYSTEM_OBJECT = $(BUILD_DIR)/system_stm32f10x.o

STARTUP_OBJECT = $(BUILD_DIR)/startup.o

OBJECTS = \
$(APP_OBJECTS) \
$(SPL_OBJECTS) \
$(SYSTEM_OBJECT) \
$(STARTUP_OBJECT)

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/spl_%.o: $(SPL_DIR)/src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(SYSTEM_OBJECT): $(SYSTEM_SOURCE) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(STARTUP_OBJECT): startup_stm32f10x_md.s | $(BUILD_DIR)
	$(CC) $(CPU) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	$(SIZE) $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

flash: $(BUILD_DIR)/$(TARGET).bin
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x08000000

clean:
	rm -f $(BUILD_DIR)/*.o
	rm -f $(BUILD_DIR)/*.elf
	rm -f $(BUILD_DIR)/*.bin

.PHONY: all clean flash
