TARGET = firmware

SRC_DIR   = src
INC_DIR   = inc
BUILD_DIR = proj

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

CPU = -mcpu=cortex-m3 -mthumb

INCLUDES = -I$(INC_DIR)

CFLAGS = $(CPU) $(INCLUDES) -ffunction-sections -fdata-sections
LDFLAGS = $(CPU) -nostartfiles -T linker.ld -Wl,--gc-sections

APP_SOURCES := $(wildcard $(SRC_DIR)/*.c)
APP_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(APP_SOURCES))
STARTUP_OBJECT = $(BUILD_DIR)/startup.o

OBJECTS = $(APP_OBJECTS) $(STARTUP_OBJECT)

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
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
	rm -rf $(BUILD_DIR)

.PHONY: all clean flash
