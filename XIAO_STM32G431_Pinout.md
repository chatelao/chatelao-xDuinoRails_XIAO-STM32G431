# XIAO STM32G431 Pinout Mapping

This document outlines the pin mapping for a Seeed Studio XIAO compatible board based on the STM32G431CBU6 microcontroller (UFQFPN-48 package). The mapping prioritizes compatibility with the standard XIAO pinout (SAMD21/RP2040) while maximizing the availability of the STM32G431's advanced features through additional pads.

## Design Philosophy

1.  **Legacy Compatibility:** The 14 standard edge pins (D0-D10, Power) are mapped to match the functions of the XIAO RP2040 and SAMD21 as closely as possible.
    *   **Analog:** D0-D3 are mapped to pins with ADC capabilities. D0 is mapped to a pin with DAC capability.
    *   **I2C:** D4 (SDA) and D5 (SCL) are mapped to the standard I2C1 interface. *Note: D5 (PA15) does not support Analog input on the G431, similar to the RP2040.*
    *   **UART:** D6 (TX) and D7 (RX) are mapped to USART1.
    *   **SPI:** D8 (SCK), D9 (MISO), and D10 (MOSI) are mapped to SPI1.
2.  **Extended Features ("Plus"):** Extra GPIOs are exposed via pads on the bottom of the board, enabling access to a second SPI bus (SPI2), a second UART (UART3), additional Analog inputs, and RGB LED control.
3.  **USB & Debug:** Standard USB-C and SWD interface pads are provided.

## Pinout Table

| Pin Name | XIAO Function | STM32 Pin | STM32 Function 1 | STM32 Function 2 | STM32 Function 3 | Reason/Notes |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Legacy Edge Pins** | | | | | | |
| **D0** | A0 / DAC | **PA4** | **DAC1_OUT1** | ADC2_IN17 | TIM3_CH2 | Matches SAMD21 DAC capability. |
| **D1** | A1 | **PA0** | ADC1_IN1 | TIM2_CH1 | | High-performance Analog input. |
| **D2** | A2 | **PA1** | ADC1_IN2 | TIM2_CH2 | | High-performance Analog input. |
| **D3** | A3 | **PA2** | ADC1_IN3 | TIM2_CH3 | LPUART1_TX | Analog input. |
| **D4** | SDA | **PB7** | **I2C1_SDA** | USART1_RX | TIM4_CH2 | Standard I2C SDA. |
| **D5** | SCL | **PA15** | **I2C1_SCL** | TIM2_CH1 | SPI1_NSS | Standard I2C SCL. (PB8 avoided due to BOOT0 conflict). No Analog. |
| **D6** | TX | **PA9** | **USART1_TX** | TIM1_CH2 | I2C2_SCL | Standard UART TX. |
| **D7** | RX | **PA10** | **USART1_RX** | TIM1_CH3 | SPI2_MISO | Standard UART RX. |
| **D8** | SCK | **PA5** | **SPI1_SCK** | ADC2_IN13 | DAC1_OUT2 | Standard SPI SCK. ADC/DAC available. |
| **D9** | MISO | **PA6** | **SPI1_MISO** | ADC2_IN3 | TIM3_CH1 | Standard SPI MISO. ADC available. |
| **D10** | MOSI | **PA7** | **SPI1_MOSI** | ADC2_IN4 | TIM3_CH2 | Standard SPI MOSI. ADC available. |
| **3V3** | Power | **3V3** | | | | 3.3V Output/Input |
| **GND** | Power | **GND** | | | | Ground |
| **5V** | Power | **5V** | | | | 5V Input (VBUS) |
| | | | | | | |
| **Onboard** | | | | | | |
| **LED_USR** | User LED | **PC13** | GPIO | | | Standard User LED (often Blue). |
| **LED_R** | RGB Red | **PB0** | TIM3_CH3 | ADC1_IN15 | | RGB LED Red Channel (PWM). |
| **LED_G** | RGB Green | **PB1** | TIM3_CH4 | ADC1_IN12 | | RGB LED Green Channel (PWM). |
| **LED_B** | RGB Blue | **PB2** | ADC2_IN12 | | | RGB LED Blue Channel (PWM/GPIO). |
| | | | | | | |
| **Bottom Pads (Plus)** | | | | | | |
| **P0** | | **PA3** | ADC1_IN4 | TIM2_CH4 | LPUART1_RX | Completes LPUART1 with D3 (PA2). Extra Analog. |
| **P1** | | **PB10** | **USART3_TX** | TIM2_CH3 | | Secondary UART TX. |
| **P2** | | **PB11** | **USART3_RX** | TIM2_CH4 | ADC1_IN14 | Secondary UART RX. Extra Analog. |
| **P3** | | **PB13** | **SPI2_SCK** | TIM1_CH1N | | Secondary SPI SCK. |
| **P4** | | **PB14** | **SPI2_MISO** | TIM1_CH2N | ADC1_IN5 | Secondary SPI MISO. Extra Analog. |
| **P5** | | **PB15** | **SPI2_MOSI** | TIM1_CH3N | ADC2_IN15 | Secondary SPI MOSI. Extra Analog. |
| **P6** | | **PB12** | **SPI2_NSS** | ADC1_IN11 | | Secondary SPI Chip Select. |
| **SWDIO** | SWD | **PA13** | SWDIO | | | Debug Data. |
| **SWCLK** | SWD | **PA14** | SWCLK | | | Debug Clock. |
| **RST** | Reset | **NRST** | | | | System Reset. |
| **GND** | Power | **GND** | | | | Ground Pad. |

## Notes

*   **Crystal Oscillator:** Pins PF0 (OSC_IN) and PF1 (OSC_OUT) are left unmapped to allow for a High Speed External (HSE) crystal if desired, or they can be used as extra GPIOs (D39/D40) if the internal HSI oscillator is used (which supports USB via CRS).
*   **BOOT0:** PB8 is the BOOT0 pin. It is not used for I2C (D5) to avoid boot mode conflicts. It can be exposed as a test point or button if needed.
*   **USB:** PA11 (D-) and PA12 (D+) are dedicated to USB.
*   **Compatibility:**
    *   **RP2040:** Matches Digital/Comm pinout exactly. D0-D3 Analog matches. D4-D5 are Digital only (like RP2040).
    *   **SAMD21:** Matches Digital/Comm pinout. Loses Analog on D4/D5 compared to SAMD21 (A4/A5).
