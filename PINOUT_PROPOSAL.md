# XIAO STM32G431 Pinout Proposal

This proposal aims to maximize compatibility with the Seeed XIAO ecosystem (SAMD21/RP2040) while leveraging the features of the STM32G431KBU6 (UFQFPN32).

## Standard XIAO Pins (D0-D10)

| Pin | XIAO Standard Function | STM32G431 Pin | Pin Name | Alternate Functions (Selected) | Notes |
|:---:|:---:|:---:|:---:|:---:|:---|
| **D0** | A0, **DAC** | 10 | **PA4** | DAC1_OUT1, ADC2_IN17, SPI1_NSS | **Changed** from PA0 to support DAC. |
| **D1** | A1 | 6 | PA0 | ADC12_IN1, TIM2_CH1 | |
| **D2** | A2 | 7 | PA1 | ADC12_IN2, TIM2_CH2 | |
| **D3** | A3 | 8 | PA2 | ADC12_IN3, LPUART1_TX, TIM2_CH3 | |
| **D4** | A4, SDA | 30 | **PB7** | I2C1_SDA, USART1_RX | Hardware I2C1 SDA. |
| **D5** | A5, SCL | 29 | **PB6** | I2C1_SCL, USART1_TX | Hardware I2C1 SCL. |
| **D6** | TX | 19 | **PA9** | USART1_TX, I2C2_SCL | Hardware USART1 TX. |
| **D7** | RX | 20 | **PA10** | USART1_RX, I2C2_SDA | Hardware USART1 RX. |
| **D8** | SCK | 11 | **PA5** | SPI1_SCK, DAC1_OUT2, ADC2_IN13 | Hardware SPI1 SCK. Also DAC2. |
| **D9** | MISO | 12 | **PA6** | SPI1_MISO, ADC2_IN3 | Hardware SPI1 MISO. |
| **D10**| MOSI | 13 | **PA7** | SPI1_MOSI, ADC2_IN4 | Hardware SPI1 MOSI. |

## On-board RGB LED

*Memory indicated RGB on PB0/PB1/PB2. However, PB2 is not available on the UFQFPN32 package (Pin 16 is VSS).*

| LED | Pin | Notes |
|:---:|:---:|:---|
| **Red** | **PB0** (Pin 14) | TIM3_CH3, OPAMP2_VINP |
| **Green**| **PB1** (Pin 15) | TIM3_CH4, OPAMP3_VOUT |
| **Blue** | **PF0** (Pin 2) | OSC_IN / GPIO. Reuses "User LED" position. |

*Note: PF0 is used to save precious GPIOs for the "Plus" pads. If discrete LEDs are used, PF0 works.*

## Xiao Plus Extra Pads (Bottom)

Due to the 32-pin package limits, only 7 extra GPIOs are fully available if using dedicated RGB pins.

| Pad | STM32 Pin | Functions | Notes |
|:---:|:---:|:---:|:---|
| **P0** | **PA3** | ADC1_IN4, **LPUART1_RX** | Good for Low Power UART. |
| **P1** | **PB3** | **SPI3_SCK**, USART2_TX | |
| **P2** | **PB4** | **SPI3_MISO**, USART2_RX | |
| **P3** | **PB5** | **SPI3_MOSI**, I2C3_SDA | Completes SPI3 with P1/P2. |
| **P4** | **PA15** | **SPI1_NSS**, I2C1_SCL | |
| **P5** | **PA8** | I2C3_SCL, TIM1_CH1 | High current timer output. |
| **P6** | **PF1** | OSC_OUT / GPIO | |
| **P7** | **PB8** | **BOOT0**, I2C1_SCL | Shared with BOOT0 button. |

*   **UART3** is not easily available on this package (Pins PB10/PB11/PC10/PC11 missing).
*   **LPUART1** is available on D3/P0 (PA2/PA3).
*   **SPI3** is available on P1/P2/P3 (PB3/PB4/PB5).

## Summary of Changes vs Netlist

1.  **D0**: Changed to **PA4** (was PA0) to enable DAC output.
2.  **D1**: Changed to **PA0** (was PA1) to shift Analog pins.
3.  **D2**: Changed to **PA1** (was PA2).
4.  **D3**: Changed to **PA2** (was PA3).
5.  **D5**: Netlist used "PA15" net name but connected to Pin 29 (PB6). Proposal confirms **PB6** for SCL.
6.  **RGB**: Integrated PB0/PB1 and PF0.
7.  **Extra**: Defined P0-P7 mapping for bottom pads.
