
TARGET    := firmware
SRC_DIR   := src
INC_DIR   := inc
BUILD_DIR := proj

CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE    := arm-none-eabi-size

CPU := -mcpu=cortex-m3 -mthumb

CFLAGS := \
	$(CPU) \
	-I$(INC_DIR) \
	-std=c11 \
	-Os \
	-g3 \
	-Wall \
	-Wextra \
	-ffreestanding \
	-fno-builtin \
	-ffunction-sections \
	-fdata-sections \
	-fno-unwind-tables \
	-fno-asynchronous-unwind-tables

LDFLAGS := \
	$(CPU) \
	-nostdlib \
	-nostartfiles \
	-T linker.ld \
	-Wl,--gc-sections \
	-Wl,-Map=$(BUILD_DIR)/$(TARGET).map

APP_SOURCES := $(wildcard $(SRC_DIR)/*.c)
APP_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(APP_SOURCES))
STARTUP_OBJECT := $(BUILD_DIR)/startup.o
OBJECTS := $(APP_OBJECTS) $(STARTUP_OBJECT)

ifeq ($(OS),Windows_NT)
MKDIR_BUILD = powershell -NoProfile -Command "New-Item -ItemType Directory -Force -Path '$(BUILD_DIR)' | Out-Null"
CLEAN_BUILD = powershell -NoProfile -Command "Remove-Item -Force -ErrorAction SilentlyContinue '$(BUILD_DIR)/*.o','$(BUILD_DIR)/*.elf','$(BUILD_DIR)/*.bin','$(BUILD_DIR)/*.hex','$(BUILD_DIR)/*.map'"
else
MKDIR_BUILD = mkdir -p $(BUILD_DIR)
CLEAN_BUILD = rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.elf $(BUILD_DIR)/*.bin $(BUILD_DIR)/*.hex $(BUILD_DIR)/*.map
endif

.PHONY: all clean flash flash-openocd flash-cube size

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex

$(BUILD_DIR):
	$(MKDIR_BUILD)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(STARTUP_OBJECT): startup_stm32f10x_md.s | $(BUILD_DIR)
	$(CC) $(CPU) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	$(SIZE) $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

size: $(BUILD_DIR)/$(TARGET).elf
	$(SIZE) $<


flash: $(BUILD_DIR)/$(TARGET).bin
	st-flash --reset write $(BUILD_DIR)/$(TARGET).bin 0x08000000


flash-openocd: $(BUILD_DIR)/$(TARGET).elf
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"

flash-cube: $(BUILD_DIR)/$(TARGET).bin
	STM32_Programmer_CLI -c port=SWD mode=UR reset=HWrst -w $(BUILD_DIR)/$(TARGET).bin 0x08000000 -v -rst

clean:
	$(CLEAN_BUILD)
