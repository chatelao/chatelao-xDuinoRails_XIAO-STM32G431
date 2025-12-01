#define MICROPY_HW_BOARD_NAME       "XIAO_STM32G431"
#define MICROPY_HW_MCU_NAME         "STM32G431KBU6"

#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_DAC       (0) // Disable DAC to save space
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_HAS_FLASH        (1) // Internal flash

// Minimal build settings to fit 128KB
#define MICROPY_ENABLE_COMPILER     (1) // Keep compiler for REPL
#define MICROPY_PY_ASYNCIO          (0)
#define MICROPY_PY_DEFLATE          (0)
#define MICROPY_PY_BINASCII         (0)
#define MICROPY_PY_HASHLIB          (0)
#define MICROPY_PY_JSON             (0)
#define MICROPY_PY_RE               (0)
#define MICROPY_PY_FRAMEBUF         (0)
#define MICROPY_PY_SOCKET           (0)
#define MICROPY_PY_NETWORK          (0)
#define MICROPY_PY_MATH             (0)
#define MICROPY_PY_CMATH            (0)
#define MICROPY_PY_STRUCT           (0)
#define MICROPY_PY_SYS              (1)
#define MICROPY_PY_IO               (1)
#define MICROPY_PY_TIME             (1)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_PY_UCTYPES          (0)
#define MICROPY_PY_THREAD           (0)

// Disable floats to save space and match MICROPY_FLOAT_IMPL=none
#define MICROPY_PY_BUILTINS_FLOAT   (0)
#define MICROPY_PY_BUILTINS_COMPLEX (0)

// Clock config
// Use HSI (16MHz) as the source for PLL
#define MICROPY_HW_CLK_USE_HSI      (1)
#define MICROPY_HW_CLK_USE_HSI48    (1) // For USB

// PLL for 170MHz from 16MHz HSI
// f_VCO = f_PLLIN * PLLN = (16 / PLLM) * PLLN
// f_PLLIN must be between 2.66 and 16 MHz.
// PLLM = 4 -> 4MHz.
// PLLN = 85 -> 340MHz.
// PLLR (SYSCLK) = 2 -> 170MHz.
// PLLQ (48MHz domain?) -> 8 -> 42.5MHz? No.
// USB uses HSI48 so we don't need PLLQ for USB.
#define MICROPY_HW_CLK_PLLM         (4)
#define MICROPY_HW_CLK_PLLN         (85)
#define MICROPY_HW_CLK_PLLP         (2)
#define MICROPY_HW_CLK_PLLQ         (8)
#define MICROPY_HW_CLK_PLLR         (2)

// 8 wait states for 170MHz
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_8

// UART
// XIAO Pin D6 (PA9) / D7 (PA10) are USART1 TX/RX
#define MICROPY_HW_UART1_TX         (pin_A9)
#define MICROPY_HW_UART1_RX         (pin_A10)
#define MICROPY_HW_UART_REPL        (PYB_UART_1)
#define MICROPY_HW_UART_REPL_BAUD   (115200)

// I2C
// PB7 is I2C1_SDA. PA15 is I2C1_SCL.
#define MICROPY_HW_I2C1_SCL         (pin_A15)
#define MICROPY_HW_I2C1_SDA         (pin_B7)

// SPI
// XIAO Pin D8 (PA5) / D9 (PA6) / D10 (PA7) are SPI1 SCK/MISO/MOSI
#define MICROPY_HW_SPI1_SCK         (pin_A5)
#define MICROPY_HW_SPI1_MISO        (pin_A6)
#define MICROPY_HW_SPI1_MOSI        (pin_A7)

// LED
#define MICROPY_HW_LED1             (pin_C14) // User LED (Blue)
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB
#define MICROPY_HW_USB_FS           (1)
