MCU_SERIES = g4
CMSIS_MCU = STM32G431xx
MICROPY_FLOAT_IMPL = none
AF_FILE = boards/stm32g474_af.csv
LD_FILES = $(BOARD_DIR)/stm32g431.ld boards/common.ld
FROZEN_MANIFEST = $(BOARD_DIR)/manifest.py

# Minimal build configuration for 128KB Flash
MICROPY_ROM_TEXT_COMPRESSION = 1
