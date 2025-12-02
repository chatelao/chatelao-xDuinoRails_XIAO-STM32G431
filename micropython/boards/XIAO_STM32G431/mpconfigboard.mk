MCU_SERIES = g4
CMSIS_MCU = STM32G431xx
MICROPY_FLOAT_IMPL = none
AF_FILE = boards/stm32g474_af.csv
LD_FILES = $(BOARD_DIR)/stm32g431.ld boards/common.ld
FROZEN_MANIFEST = $(BOARD_DIR)/manifest.py

# Minimal build configuration for 128KB Flash
MICROPY_ROM_TEXT_COMPRESSION = 1

# Disable modules in Makefile to prevent compilation of their source files
MICROPY_PY_LWIP = 0
MICROPY_PY_NETWORK = 0
MICROPY_PY_ASYNCIO = 0
MICROPY_PY_JSON = 0
MICROPY_PY_RE = 0
MICROPY_PY_HASHLIB = 0
MICROPY_PY_BINASCII = 0
MICROPY_PY_FRAMEBUF = 0
MICROPY_PY_MATH = 0
MICROPY_PY_CMATH = 0
MICROPY_PY_STRUCT = 0
MICROPY_PY_UCTYPES = 0
MICROPY_PY_THREAD = 0

# Force defines via CFLAGS to override any port-level defaults or header ordering issues
# We explicitly disable MACHINE_IDLE to prevent "used but never defined" error in modmachine.c
CFLAGS += -DMICROPY_PY_MACHINE_IDLE=0 -DMICROPY_PY_MACHINE_PULSE=0 -DMICROPY_PY_LWIP=0
