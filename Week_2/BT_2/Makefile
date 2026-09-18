# ============================================================
# TARGET
# ============================================================

TARGET = firmware


# ============================================================
# DIRECTORIES
# ============================================================

SRC_DIR   = src
INC_DIR   = inc
LIB_DIR   = Lib
BUILD_DIR = proj

SPL_DIR   = $(LIB_DIR)/STM32F10x_StdPeriph_Driver
CMSIS_DIR = $(LIB_DIR)/CMSIS/CM3


# ============================================================
# TOOLCHAIN
# ============================================================

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

STINFO  = st-info
STFLASH = st-flash


# ============================================================
# MCU
# ============================================================

CPU = -mcpu=cortex-m3 -mthumb


# ============================================================
# DEFINES
# ============================================================

DEFS = \
-DSTM32F10X_MD \
-DUSE_STDPERIPH_DRIVER


# ============================================================
# INCLUDE PATHS
# ============================================================

INCLUDES = \
-I$(INC_DIR) \
-I$(SPL_DIR)/inc \
-I$(CMSIS_DIR)/CoreSupport \
-I$(CMSIS_DIR)/DeviceSupport/ST/STM32F10x


# ============================================================
# COMPILER FLAGS
# ============================================================

CFLAGS = \
$(CPU) \
$(DEFS) \
$(INCLUDES) \
-ffunction-sections \
-fdata-sections


# ============================================================
# LINKER FLAGS
# ============================================================

LDFLAGS = \
$(CPU) \
-nostartfiles \
-T linker.ld \
-Wl,--gc-sections


# ============================================================
# SOURCE FILES
# ============================================================

# User source files: src/*.c
APP_SOURCES := $(wildcard $(SRC_DIR)/*.c)

# STM32 SPL source
SPL_SOURCES := $(wildcard $(SPL_DIR)/src/*.c)

# CMSIS system source
SYSTEM_SOURCE = \
$(CMSIS_DIR)/DeviceSupport/ST/STM32F10x/system_stm32f10x.c

# STM32F103C8T6 = Medium Density
STARTUP_SOURCE = \
$(CMSIS_DIR)/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_md.s


# ============================================================
# OBJECT FILES
# ============================================================

APP_OBJECTS := \
$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(APP_SOURCES))

SPL_OBJECTS := \
$(patsubst $(SPL_DIR)/src/%.c,$(BUILD_DIR)/spl_%.o,$(SPL_SOURCES))

SYSTEM_OBJECT  = $(BUILD_DIR)/system_stm32f10x.o
STARTUP_OBJECT = $(BUILD_DIR)/startup.o

OBJECTS = \
$(APP_OBJECTS) \
$(SPL_OBJECTS) \
$(SYSTEM_OBJECT) \
$(STARTUP_OBJECT)


# ============================================================
# DEFAULT BUILD
# ============================================================

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin


# ============================================================
# CREATE BUILD DIRECTORY
# ============================================================

$(BUILD_DIR):
ifeq ($(OS),Windows_NT)
	powershell -NoProfile -Command "New-Item -ItemType Directory -Force -Path '$(BUILD_DIR)' | Out-Null"
else
	mkdir -p $(BUILD_DIR)
endif


# ============================================================
# APPLICATION SOURCE
# ============================================================

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# SPL SOURCE
# ============================================================

$(BUILD_DIR)/spl_%.o: $(SPL_DIR)/src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# CMSIS SYSTEM
# ============================================================

$(SYSTEM_OBJECT): $(SYSTEM_SOURCE) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# STARTUP
# ============================================================

$(STARTUP_OBJECT): $(STARTUP_SOURCE) | $(BUILD_DIR)
	$(CC) $(CPU) -c $< -o $@


# ============================================================
# LINK ELF
# ============================================================

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	$(SIZE) $@


# ============================================================
# ELF -> BIN
# ============================================================

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@


# ============================================================
# ST-LINK CHECK
# ============================================================

check:
	$(STINFO) --probe


# ============================================================
# FLASH STM32
# ============================================================

flash: $(BUILD_DIR)/$(TARGET).bin
	$(STINFO) --probe
	$(STFLASH) write $(BUILD_DIR)/$(TARGET).bin 0x08000000


# ============================================================
# CLEAN
# ============================================================

clean:
ifeq ($(OS),Windows_NT)
	powershell -NoProfile -Command "if (Test-Path '$(BUILD_DIR)') { Remove-Item -Recurse -Force '$(BUILD_DIR)' }"
else
	rm -rf $(BUILD_DIR)
endif


# ============================================================
# PHONY
# ============================================================

.PHONY: all clean check flash
