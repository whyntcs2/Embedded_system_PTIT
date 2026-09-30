
# =====================================
# STM32F103C8T6 - MAKEFILE
# =====================================

OPENOCD = D:/Downloads/xpack-openocd-0.12.0-7-win32-x64/xpack-openocd-0.12.0-7/bin/openocd.exe

OPENOCD_SCRIPTS = D:/Downloads/xpack-openocd-0.12.0-7-win32-x64/xpack-openocd-0.12.0-7/openocd/scripts
TARGET = firmware
BUILD = build

CUBE = STM32CubeF1
CMSIS = $(CUBE)/Drivers/CMSIS

DEVICE = $(CMSIS)/Device/ST/STM32F1xx

CORE_INC = $(CMSIS)/Include
DEVICE_INC = $(DEVICE)/Include

SYSTEM_SRC = $(DEVICE)/Source/Templates/system_stm32f1xx.c
STARTUP_SRC = $(DEVICE)/Source/Templates/gcc/startup_stm32f103xb.s

LDSCRIPT = linker/STM32F103C8.ld


# =====================================
# TOOLCHAIN
# =====================================

PREFIX = arm-none-eabi-

CC = $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy
SIZE = $(PREFIX)size


# =====================================
# COMPILER OPTIONS
# =====================================

CPU = -mcpu=cortex-m3 -mthumb

DEFS = -DSTM32F103xB

INCLUDES = \
    -I$(CORE_INC) \
    -I$(DEVICE_INC)

CFLAGS = \
    $(CPU) \
    $(DEFS) \
    $(INCLUDES) \
    -std=c11 \
    -O2 \
    -g3 \
    -Wall \
    -Wextra \
    -ffunction-sections \
    -fdata-sections \
    -MMD \
    -MP

ASFLAGS = $(CPU) -g3

LDFLAGS = \
    $(CPU) \
    -T$(LDSCRIPT) \
    -Wl,--gc-sections \
    -Wl,-Map=$(BUILD)/$(TARGET).map \
    -specs=nano.specs \
    -specs=nosys.specs


# =====================================
# OBJECT FILES
# =====================================

OBJS = \
    $(BUILD)/main.o \
    $(BUILD)/system_stm32f1xx.o \
    $(BUILD)/startup_stm32f103xb.o


# =====================================
# BUILD RULES
# =====================================

all: $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).bin

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/main.o: src/main.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/system_stm32f1xx.o: $(SYSTEM_SRC) | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/startup_stm32f103xb.o: $(STARTUP_SRC) | $(BUILD)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	$(CC) $(OBJS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@


# =====================================
# FLASH BY ST-LINK
# =====================================

flash: all
	"$(OPENOCD)" -s "$(OPENOCD_SCRIPTS)" -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program build/firmware.elf verify reset exit"


# =====================================
# CLEAN
# =====================================

clean:
	rm -rf $(BUILD)

-include $(OBJS:.o=.d)

.PHONY: all flash clean
